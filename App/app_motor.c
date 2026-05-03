/**
  ******************************************************************************
  * @file    app_motor.c
  * @brief   Motor PWM control - 16 channels, 50Hz
  ******************************************************************************
  */
#include "app_motor.h"

void APP_Motor_Init(void)
{
  /* TIM2: PA0-PA3 (group 1) */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

  /* TIM4: PD12-PD15 (group 2) */
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  /* TIM5: PH10-PH12, PI0 (group 3) */
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

  /* TIM8: PI5-PI7, PI2 (group 4) */
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
}

void APP_Motor_SetDuty(TIM_HandleTypeDef *tim, uint32_t channel, float duty)
{
  uint32_t ccr = (uint32_t)(PWM_RESOLUTION * duty) - 1;
  switch(channel)
  {
    case TIM_CHANNEL_1: tim->Instance->CCR1 = ccr; break;
    case TIM_CHANNEL_2: tim->Instance->CCR2 = ccr; break;
    case TIM_CHANNEL_3: tim->Instance->CCR3 = ccr; break;
    case TIM_CHANNEL_4: tim->Instance->CCR4 = ccr; break;
  }
}
