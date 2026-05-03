/**
  ******************************************************************************
  * @file    app_imu.h
  * @brief   IMU application module
  ******************************************************************************
  */
#ifndef __APP_IMU_H__
#define __APP_IMU_H__

#include "bsp_imu.h"

void APP_IMU_Init(void);
void APP_IMU_Update(void);
void APP_IMU_GetAttitude(float *roll, float *pitch, float *yaw);

#endif
