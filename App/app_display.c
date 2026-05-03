/**
  ******************************************************************************
  * @file    app_display.c
  * @brief   OLED display application
  ******************************************************************************
  */
#include "app_display.h"

void APP_Display_Init(void)
{
  oled_init();
  oled_clear(Pen_Clear);
  oled_LOGO();
  oled_refresh_gram();
}

void APP_Display_ShowString(uint8_t row, uint8_t col, const char *str)
{
  oled_showstring(row, col, (uint8_t *)str);
  oled_refresh_gram();
}

void APP_Display_ShowNum(uint8_t row, uint8_t col, uint32_t num, uint8_t len)
{
  oled_shownum(row, col, num, 0, len);
  oled_refresh_gram();
}

void APP_Display_Clear(void)
{
  oled_clear(Pen_Clear);
  oled_refresh_gram();
}
