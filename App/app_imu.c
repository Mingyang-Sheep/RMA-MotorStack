/**
  ******************************************************************************
  * @file    app_imu.c
  * @brief   IMU application module - wraps MPU6500 + AHRS
  ******************************************************************************
  */
#include "app_imu.h"
#include "spi.h"
#include "usart.h"
#include "main.h"
#include <stdio.h>

extern UART_HandleTypeDef huart6;

void APP_IMU_Init(void)
{
  mpu_device_init();
  init_quaternion();
}

void APP_IMU_Update(void)
{
  mpu_get_data();
  imu_ahrs_update();
  imu_attitude_update();
}

void APP_IMU_GetAttitude(float *roll, float *pitch, float *yaw)
{
  *roll  = imu.rol;
  *pitch = imu.pit;
  *yaw   = imu.yaw;
}
