/**
  ******************************************************************************
  * @file    app_c620.h
  * @brief   C620 ESC driver for M3508 motors over CAN.
  ******************************************************************************
  */
#ifndef __APP_C620_H__
#define __APP_C620_H__

#include "main.h"

#define C620_MOTOR_COUNT              4U
#define C620_CAN_TX_ID_MOTOR_1_TO_4   0x200U
#define C620_CAN_RX_ID_MOTOR_1        0x201U
#define C620_CAN_RX_ID_MOTOR_4        0x204U

#define C620_MEDIUM_SPEED_RPM         1500
#define C620_FAST_SPEED_RPM           3000

typedef enum
{
  C620_SPEED_LEVEL_STOP = 0,
  C620_SPEED_LEVEL_MEDIUM,
  C620_SPEED_LEVEL_FAST,
  C620_SPEED_LEVEL_COUNT
} C620_SpeedLevel_t;

typedef struct
{
  uint16_t angle;
  uint16_t last_angle;
  uint16_t offset_angle;
  int32_t  round_count;
  int32_t  total_angle;
  int16_t  speed_rpm;
  int16_t  torque_current;
  uint8_t  temperature;
  uint8_t  online;
  uint32_t msg_count;
  uint32_t last_update_ms;
  int16_t  target_speed_rpm;
  int16_t  current_cmd;
} C620_MotorFeedback_t;

void APP_C620_Init(void);
void APP_C620_Update(void);

void APP_C620_SetSpeedLevel(C620_SpeedLevel_t level);
C620_SpeedLevel_t APP_C620_GetSpeedLevel(void);
C620_SpeedLevel_t APP_C620_NextSpeedLevel(void);

void APP_C620_SetMotorSpeed(uint8_t motor_id, int16_t speed_rpm);
C620_MotorFeedback_t APP_C620_GetMotorFeedback(uint8_t motor_id);

uint8_t APP_C620_ProcessCanRx(CAN_HandleTypeDef *hcan,
                              const CAN_RxHeaderTypeDef *rx_header,
                              const uint8_t rx_data[8]);

#endif
