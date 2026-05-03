/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Unified header for RMA-MotorStack project
  ******************************************************************************
  */

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private define ------------------------------------------------------------*/

/* LED pin definitions */
#define LED_RED_Pin           GPIO_PIN_11
#define LED_RED_GPIO_Port     GPIOE
#define LED_GREEN_Pin         GPIO_PIN_14
#define LED_GREEN_GPIO_Port   GPIOF
#define LED_G_GPIO_Port       GPIOF
#define LED_G_Pin             GPIO_PIN_14

/* OLED pin definitions */
#define OLED_DC_Pin           GPIO_PIN_4
#define OLED_DC_GPIO_Port     GPIOE
#define OLED_RST_Pin          GPIO_PIN_5
#define OLED_RST_GPIO_Port    GPIOE

/* PWM configuration */
#define PWM_RESOLUTION        10000
#define PWM_FREQUENCE         50
#define PWM_DEFAULT_DUTY      5000
#define APB1_TIMER_CLOCKS     84000000
#define APB2_TIMER_CLOCKS     168000000
#define TIM_PSC_APB1          ((APB1_TIMER_CLOCKS/PWM_FREQUENCE)/PWM_RESOLUTION - 1)
#define TIM_PSC_APB2          ((APB2_TIMER_CLOCKS/PWM_FREQUENCE)/PWM_RESOLUTION - 1)

/* USB VCP */
#define USB_VCP_RX_BUF_SIZE   2048

/* Helper macros */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* ########################## Assert Selection ############################## */
/* #define USE_FULL_ASSERT    1U */

void _Error_Handler(char *, int);
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
