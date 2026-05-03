/**
  ******************************************************************************
  * @file    app_display.h
  * @brief   OLED display application module
  ******************************************************************************
  */
#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__

#include "bsp_oled.h"

void APP_Display_Init(void);
void APP_Display_ShowString(uint8_t row, uint8_t col, const char *str);
void APP_Display_ShowNum(uint8_t row, uint8_t col, uint32_t num, uint8_t len);
void APP_Display_Clear(void);

#endif
