/**
  ******************************************************************************
  * @file    app_c620.c
  * @brief   C620 ESC motor control via CAN bus
  *
  *          C620 CAN protocol:
  *            - CAN ID 0x200 for motors 1-4
  *            - 8 bytes = 4 x int16_t current (high byte first)
  *            - Current range: -10000 ~ +10000 (maps to ~ -20A ~ +20A)
  *
  *          Button (PA0 / WKUP):
  *            - Short press cycles speed: STOP -> SLOW -> MEDIUM -> FAST -> STOP
  ******************************************************************************
  */
#include "app_c620.h"
#include "can.h"
#include "gpio.h"
#include "bsp_oled.h"
#include <stdio.h>

extern CAN_HandleTypeDef hcan1;

/* Speed level parameters (current value sent to C620) */
static const int16_t speed_currents[] = {
    0,      /* STOP   */
    3000,   /* SLOW   ~6A  */
    6000,   /* MEDIUM ~12A */
    9000    /* FAST   ~18A */
};
static const char *speed_labels[] = {
    "STOP", "SLOW", "MEDIUM", "FAST"
};

static uint8_t  speed_level = C620_SPEED_STOP;
static uint8_t  button_last = 1;       /* Last button state (1 = released) */
static uint32_t button_debounce = 0;   /* Debounce timer (ms) */
static uint32_t last_can_tx = 0;       /* Last CAN TX time */
static uint32_t last_display = 0;      /* Last OLED update time */

/**
  * @brief  Initialize C620 driver
  *         - Configure PA0 as GPIO input (button)
  *         - Display initial state on OLED
  */
void APP_C620_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOA clock (already enabled by MX_GPIO_Init) */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA0 as input with pull-down for button */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Show initial speed on OLED */
  oled_clear(Pen_Clear);
  oled_showstring(0, 0, (uint8_t *)"C620 Motor Ctrl");
  oled_showstring(2, 0, (uint8_t *)"Speed: STOP");
  oled_showstring(4, 0, (uint8_t *)"Press KEY (PA0)");
  oled_showstring(5, 0, (uint8_t *)"to change speed");
  oled_refresh_gram();
}

/**
  * @brief  Send current command to C620 via CAN
  */
static void C620_SendCurrent(int16_t current)
{
  CAN_TxHeaderTypeDef tx_header;
  uint8_t tx_data[8];
  uint32_t tx_mailbox;

  tx_header.StdId = C620_CAN_ID;
  tx_header.ExtId = 0;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = 8;
  tx_header.TransmitGlobalTime = DISABLE;

  /* Motor 1 current: high byte first (big-endian) */
  tx_data[0] = (uint8_t)(current >> 8);
  tx_data[1] = (uint8_t)(current & 0xFF);
  /* Motor 2-4: zero current */
  tx_data[2] = 0;
  tx_data[3] = 0;
  tx_data[4] = 0;
  tx_data[5] = 0;
  tx_data[6] = 0;
  tx_data[7] = 0;

  HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &tx_mailbox);
}

/**
  * @brief  Read button with debounce, update speed level
  *         Returns 1 if speed changed
  */
static uint8_t C620_ButtonCheck(uint32_t now_ms)
{
  uint8_t raw = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

  /* Button pressed: active high, debounce 50ms */
  if (raw == 1 && button_last == 0 && (now_ms - button_debounce) > 50)
  {
    button_debounce = now_ms;
    button_last = 1;

    /* Cycle speed level */
    speed_level++;
    if (speed_level > C620_SPEED_FAST)
    {
      speed_level = C620_SPEED_STOP;
    }
    return 1;
  }

  if (raw == 0)
  {
    button_last = 0;
  }

  return 0;
}

/**
  * @brief  Main update function: check button, send CAN, update display
  *         Call periodically from main loop (every 5-10ms)
  */
void APP_C620_Update(void)
{
  uint32_t now = HAL_GetTick();

  /* Check button every cycle */
  if (C620_ButtonCheck(now))
  {
    /* Stop motor transmission when speed is STOP */
    if (speed_level == C620_SPEED_STOP)
    {
      C620_SendCurrent(0);
    }
  }

  /* Send CAN command every 5ms (200Hz control rate) */
  if (speed_level != C620_SPEED_STOP && (now - last_can_tx) >= 5)
  {
    C620_SendCurrent(speed_currents[speed_level]);
    last_can_tx = now;
  }

  /* Update OLED display every 200ms */
  if ((now - last_display) >= 200)
  {
    last_display = now;

    char buf[32];
    sprintf(buf, "Speed: %-6s", speed_labels[speed_level]);
    oled_showstring(2, 0, (uint8_t *)buf);

    sprintf(buf, "Cur:   %-6d", (int)speed_currents[speed_level]);
    oled_showstring(3, 0, (uint8_t *)buf);

    oled_refresh_gram();
  }
}
