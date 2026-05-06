/**
  ******************************************************************************
  * @file    app_c620.c
  * @brief   C620 ESC driver for M3508 motors over CAN.
  *
  *          C620 current command:
  *            StdId 0x200 controls motor ID 1-4.
  *            Data[0..7] are four signed int16 current commands, big-endian.
  *
  *          M3508 feedback:
  *            StdId 0x201-0x204 map to motor ID 1-4.
  *            Data[0..1] angle, Data[2..3] speed rpm,
  *            Data[4..5] torque current, Data[6] temperature.
  ******************************************************************************
  */
#include "app_c620.h"
#include "can.h"
#include "bsp_oled.h"
#include <stdio.h>

extern CAN_HandleTypeDef hcan1;

#define C620_CONTROL_PERIOD_MS         10U
#define C620_DISPLAY_PERIOD_MS         200U
#define C620_FEEDBACK_TIMEOUT_MS       100U
#define C620_ENCODER_RANGE             8192
#define C620_ENCODER_HALF_RANGE        4096
#define C620_MAX_TARGET_SPEED_RPM      8000
#define C620_MAX_CURRENT_CMD           10000
#define C620_PID_INTEGRAL_LIMIT        20000.0f
#define C620_PID_KP                    1.5f
#define C620_PID_KI                    0.1f
#define C620_PID_KD                    0.0f

typedef struct
{
  float integral;
  float last_error;
} C620_Pid_t;

static volatile C620_MotorFeedback_t c620_motor[C620_MOTOR_COUNT];
static C620_Pid_t c620_speed_pid[C620_MOTOR_COUNT];
static CAN_HandleTypeDef *c620_can = &hcan1;
static C620_SpeedLevel_t c620_speed_level = C620_SPEED_LEVEL_STOP;
static uint32_t c620_last_control_ms = 0U;
static uint32_t c620_last_display_ms = 0U;

static const int16_t c620_level_speed_rpm[C620_SPEED_LEVEL_COUNT] =
{
  0,
  C620_MEDIUM_SPEED_RPM,
  C620_FAST_SPEED_RPM
};

static const char *c620_level_label[C620_SPEED_LEVEL_COUNT] =
{
  "STOP",
  "MED",
  "FAST"
};

static void C620_LimitFloat(float *value, float limit)
{
  if (*value > limit)
  {
    *value = limit;
  }
  else if (*value < -limit)
  {
    *value = -limit;
  }
}

static int16_t C620_LimitInt16(int16_t value, int16_t limit)
{
  if (value > limit)
  {
    return limit;
  }
  if (value < -limit)
  {
    return (int16_t)-limit;
  }
  return value;
}

static uint16_t C620_MakeUint16(uint8_t high, uint8_t low)
{
  return (uint16_t)(((uint16_t)high << 8) | (uint16_t)low);
}

static int16_t C620_MakeInt16(uint8_t high, uint8_t low)
{
  return (int16_t)C620_MakeUint16(high, low);
}

static void C620_ResetPid(uint8_t motor_index)
{
  c620_speed_pid[motor_index].integral = 0.0f;
  c620_speed_pid[motor_index].last_error = 0.0f;
}

static void C620_ClearFeedback(uint8_t motor_index)
{
  c620_motor[motor_index].angle = 0U;
  c620_motor[motor_index].last_angle = 0U;
  c620_motor[motor_index].offset_angle = 0U;
  c620_motor[motor_index].round_count = 0;
  c620_motor[motor_index].total_angle = 0;
  c620_motor[motor_index].speed_rpm = 0;
  c620_motor[motor_index].torque_current = 0;
  c620_motor[motor_index].temperature = 0U;
  c620_motor[motor_index].online = 0U;
  c620_motor[motor_index].msg_count = 0U;
  c620_motor[motor_index].last_update_ms = 0U;
  c620_motor[motor_index].target_speed_rpm = 0;
  c620_motor[motor_index].current_cmd = 0;
}

static int16_t C620_SpeedPidCalc(uint8_t motor_index, int16_t target_rpm, int16_t speed_rpm)
{
  C620_Pid_t *pid = &c620_speed_pid[motor_index];
  float error = (float)target_rpm - (float)speed_rpm;
  float output;

  pid->integral += error;
  C620_LimitFloat(&pid->integral, C620_PID_INTEGRAL_LIMIT);

  output = C620_PID_KP * error
         + C620_PID_KI * pid->integral
         + C620_PID_KD * (error - pid->last_error);

  pid->last_error = error;
  C620_LimitFloat(&output, (float)C620_MAX_CURRENT_CMD);

  return (int16_t)output;
}

static uint8_t C620_MotorIsOnline(uint8_t motor_index, uint32_t now_ms)
{
  uint32_t last_update_ms = c620_motor[motor_index].last_update_ms;

  if (c620_motor[motor_index].msg_count == 0U)
  {
    return 0U;
  }

  return ((now_ms - last_update_ms) <= C620_FEEDBACK_TIMEOUT_MS) ? 1U : 0U;
}

static HAL_StatusTypeDef C620_SendCurrents(const int16_t current[C620_MOTOR_COUNT])
{
  CAN_TxHeaderTypeDef tx_header;
  uint8_t tx_data[8];
  uint32_t tx_mailbox;
  uint8_t i;

  if (c620_can == 0)
  {
    return HAL_ERROR;
  }

  if (HAL_CAN_GetTxMailboxesFreeLevel(c620_can) == 0U)
  {
    return HAL_BUSY;
  }

  tx_header.StdId = C620_CAN_TX_ID_MOTOR_1_TO_4;
  tx_header.ExtId = 0U;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = 8U;
  tx_header.TransmitGlobalTime = DISABLE;

  for (i = 0U; i < C620_MOTOR_COUNT; i++)
  {
    int16_t limited_current = C620_LimitInt16(current[i], C620_MAX_CURRENT_CMD);
    tx_data[i * 2U] = (uint8_t)(limited_current >> 8);
    tx_data[i * 2U + 1U] = (uint8_t)limited_current;
  }

  return HAL_CAN_AddTxMessage(c620_can, &tx_header, tx_data, &tx_mailbox);
}

static void C620_UpdateDisplay(void)
{
  char line[32];
  C620_MotorFeedback_t motor = APP_C620_GetMotorFeedback(1U);

  oled_showstring(0, 0, (uint8_t *)"C620 M3508 CAN");

  sprintf(line, "Level:%-4s        ", c620_level_label[c620_speed_level]);
  oled_showstring(1, 0, (uint8_t *)line);

  sprintf(line, "Tar:%5d rpm     ", (int)motor.target_speed_rpm);
  oled_showstring(2, 0, (uint8_t *)line);

  sprintf(line, "Fdb:%5d rpm     ", (int)motor.speed_rpm);
  oled_showstring(3, 0, (uint8_t *)line);

  sprintf(line, "Cur:%6d        ", (int)motor.current_cmd);
  oled_showstring(4, 0, (uint8_t *)line);

  sprintf(line, "Tmp:%3u %s      ", (unsigned int)motor.temperature,
          motor.online ? "ON " : "OFF");
  oled_showstring(5, 0, (uint8_t *)line);

  oled_refresh_gram();
}

void APP_C620_Init(void)
{
  uint8_t i;

  for (i = 0U; i < C620_MOTOR_COUNT; i++)
  {
    C620_ClearFeedback(i);
    C620_ResetPid(i);
  }

  c620_can = &hcan1;
  c620_speed_level = C620_SPEED_LEVEL_STOP;
  c620_last_control_ms = HAL_GetTick();
  c620_last_display_ms = HAL_GetTick();

  oled_clear(Pen_Clear);
  C620_UpdateDisplay();
}

void APP_C620_SetMotorSpeed(uint8_t motor_id, int16_t speed_rpm)
{
  uint8_t motor_index;

  if ((motor_id == 0U) || (motor_id > C620_MOTOR_COUNT))
  {
    return;
  }

  motor_index = (uint8_t)(motor_id - 1U);
  c620_motor[motor_index].target_speed_rpm =
    C620_LimitInt16(speed_rpm, C620_MAX_TARGET_SPEED_RPM);

  if (speed_rpm == 0)
  {
    C620_ResetPid(motor_index);
  }
}

void APP_C620_SetSpeedLevel(C620_SpeedLevel_t level)
{
  if (level >= C620_SPEED_LEVEL_COUNT)
  {
    level = C620_SPEED_LEVEL_STOP;
  }

  c620_speed_level = level;
  APP_C620_SetMotorSpeed(1U, c620_level_speed_rpm[level]);
}

C620_SpeedLevel_t APP_C620_GetSpeedLevel(void)
{
  return c620_speed_level;
}

C620_SpeedLevel_t APP_C620_NextSpeedLevel(void)
{
  C620_SpeedLevel_t next_level = (C620_SpeedLevel_t)(c620_speed_level + 1U);

  if (next_level >= C620_SPEED_LEVEL_COUNT)
  {
    next_level = C620_SPEED_LEVEL_STOP;
  }

  APP_C620_SetSpeedLevel(next_level);
  return next_level;
}

C620_MotorFeedback_t APP_C620_GetMotorFeedback(uint8_t motor_id)
{
  C620_MotorFeedback_t copy = {0};
  uint8_t motor_index;

  if ((motor_id == 0U) || (motor_id > C620_MOTOR_COUNT))
  {
    return copy;
  }

  motor_index = (uint8_t)(motor_id - 1U);
  copy.angle = c620_motor[motor_index].angle;
  copy.last_angle = c620_motor[motor_index].last_angle;
  copy.offset_angle = c620_motor[motor_index].offset_angle;
  copy.round_count = c620_motor[motor_index].round_count;
  copy.total_angle = c620_motor[motor_index].total_angle;
  copy.speed_rpm = c620_motor[motor_index].speed_rpm;
  copy.torque_current = c620_motor[motor_index].torque_current;
  copy.temperature = c620_motor[motor_index].temperature;
  copy.online = c620_motor[motor_index].online;
  copy.msg_count = c620_motor[motor_index].msg_count;
  copy.last_update_ms = c620_motor[motor_index].last_update_ms;
  copy.target_speed_rpm = c620_motor[motor_index].target_speed_rpm;
  copy.current_cmd = c620_motor[motor_index].current_cmd;

  return copy;
}

uint8_t APP_C620_ProcessCanRx(CAN_HandleTypeDef *hcan,
                              const CAN_RxHeaderTypeDef *rx_header,
                              const uint8_t rx_data[8])
{
  uint8_t motor_index;
  uint16_t angle;
  int32_t delta_angle;

  if ((rx_header == 0) || (rx_data == 0))
  {
    return 0U;
  }

  if ((rx_header->IDE != CAN_ID_STD) || (rx_header->RTR != CAN_RTR_DATA))
  {
    return 0U;
  }

  if ((rx_header->StdId < C620_CAN_RX_ID_MOTOR_1) ||
      (rx_header->StdId > C620_CAN_RX_ID_MOTOR_4))
  {
    return 0U;
  }

  if (rx_header->DLC < 7U)
  {
    return 1U;
  }

  c620_can = hcan;
  motor_index = (uint8_t)(rx_header->StdId - C620_CAN_RX_ID_MOTOR_1);
  angle = C620_MakeUint16(rx_data[0], rx_data[1]);

  if (c620_motor[motor_index].msg_count == 0U)
  {
    c620_motor[motor_index].offset_angle = angle;
    c620_motor[motor_index].last_angle = angle;
    c620_motor[motor_index].round_count = 0;
  }
  else
  {
    delta_angle = (int32_t)angle - (int32_t)c620_motor[motor_index].angle;
    c620_motor[motor_index].last_angle = c620_motor[motor_index].angle;

    if (delta_angle > C620_ENCODER_HALF_RANGE)
    {
      c620_motor[motor_index].round_count--;
    }
    else if (delta_angle < -C620_ENCODER_HALF_RANGE)
    {
      c620_motor[motor_index].round_count++;
    }
  }

  c620_motor[motor_index].angle = angle;
  c620_motor[motor_index].speed_rpm = C620_MakeInt16(rx_data[2], rx_data[3]);
  c620_motor[motor_index].torque_current = C620_MakeInt16(rx_data[4], rx_data[5]);
  c620_motor[motor_index].temperature = rx_data[6];
  c620_motor[motor_index].total_angle =
    c620_motor[motor_index].round_count * C620_ENCODER_RANGE
    + (int32_t)c620_motor[motor_index].angle
    - (int32_t)c620_motor[motor_index].offset_angle;
  c620_motor[motor_index].last_update_ms = HAL_GetTick();
  c620_motor[motor_index].online = 1U;
  c620_motor[motor_index].msg_count++;

  return 1U;
}

void APP_C620_Update(void)
{
  uint32_t now_ms = HAL_GetTick();
  int16_t current[C620_MOTOR_COUNT] = {0};
  uint8_t i;

  if ((now_ms - c620_last_control_ms) >= C620_CONTROL_PERIOD_MS)
  {
    c620_last_control_ms = now_ms;

    for (i = 0U; i < C620_MOTOR_COUNT; i++)
    {
      int16_t target_rpm = c620_motor[i].target_speed_rpm;
      uint8_t online = C620_MotorIsOnline(i, now_ms);

      c620_motor[i].online = online;

      if ((target_rpm == 0) || (online == 0U))
      {
        current[i] = 0;
        C620_ResetPid(i);
      }
      else
      {
        current[i] = C620_SpeedPidCalc(i, target_rpm, c620_motor[i].speed_rpm);
      }

      c620_motor[i].current_cmd = current[i];
    }

    (void)C620_SendCurrents(current);
  }

  if ((now_ms - c620_last_display_ms) >= C620_DISPLAY_PERIOD_MS)
  {
    c620_last_display_ms = now_ms;
    C620_UpdateDisplay();
  }
}
