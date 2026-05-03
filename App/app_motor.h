/**
  ******************************************************************************
  * @file    app_motor.h
  * @brief   Motor PWM control application module
  ******************************************************************************
  */
#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__

#include "main.h"
#include "tim.h"

#define PWM_RESOLUTION 10000

void APP_Motor_Init(void);
void APP_Motor_SetDuty(TIM_HandleTypeDef *tim, uint32_t channel, float duty);

#endif
