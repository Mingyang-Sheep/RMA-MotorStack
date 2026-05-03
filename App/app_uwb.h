/**
  ******************************************************************************
  * @file    app_uwb.h
  * @brief   UWB positioning application module
  ******************************************************************************
  */
#ifndef __APP_UWB_H__
#define __APP_UWB_H__

#include "bsp_can.h"

void APP_UWB_Init(void);
void APP_UWB_GetData(uwb_info_t *out);

#endif
