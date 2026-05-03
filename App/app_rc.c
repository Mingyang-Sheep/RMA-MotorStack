/**
  ******************************************************************************
  * @file    app_rc.c
  * @brief   Remote control application - DBUS receiver
  ******************************************************************************
  */
#include "app_rc.h"

extern rc_info_t rc;

void APP_RC_Init(void)
{
  dbus_uart_init();
}

void APP_RC_GetData(rc_info_t *out)
{
  out->ch1 = rc.ch1;
  out->ch2 = rc.ch2;
  out->ch3 = rc.ch3;
  out->ch4 = rc.ch4;
  out->sw1 = rc.sw1;
  out->sw2 = rc.sw2;
}
