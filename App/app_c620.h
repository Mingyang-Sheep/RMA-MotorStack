/**
  ******************************************************************************
  * @file    app_c620.h
  * @brief   C620 ESC motor control via CAN bus
  *          Button (PA0) cycles through 4 speed levels
  ******************************************************************************
  */
#ifndef __APP_C620_H__
#define __APP_C620_H__

#include "main.h"

/* Speed level definitions */
#define C620_SPEED_STOP    0
#define C620_SPEED_SLOW    1
#define C620_SPEED_MEDIUM  2
#define C620_SPEED_FAST    3

/* CAN ID for motor 1-4 control */
#define C620_CAN_ID        0x200

void APP_C620_Init(void);
void APP_C620_Update(void);

#endif
