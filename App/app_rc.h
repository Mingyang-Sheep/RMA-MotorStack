/**
  ******************************************************************************
  * @file    app_rc.h
  * @brief   Remote control (DBUS) application module
  ******************************************************************************
  */
#ifndef __APP_RC_H__
#define __APP_RC_H__

#include "bsp_uart.h"

void APP_RC_Init(void);
void APP_RC_GetData(rc_info_t *rc);

#endif
