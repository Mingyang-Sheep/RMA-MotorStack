/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Unified main entry point for RMA-MotorStack
  *                   RoboMaster Development Board Type A (STM32F427IIHx)
  ******************************************************************************
  */
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "gpio.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "fatfs.h"

/* BSP includes */
#include "bsp_imu.h"
#include "bsp_uart.h"
#include "bsp_can.h"
#include "bsp_oled.h"
#include "bsp_driver_sd.h"
#include "app_c620.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();        /* OLED display */
  MX_SPI5_Init();        /* MPU6500 IMU */
  MX_USART1_UART_Init();  /* DBUS remote control */
  MX_USART6_UART_Init();  /* Debug serial */
  /* MX_TIM2_Init(); */    /* PWM group 1: PA0-PA3 - disabled, PA0 used as C620 button */
  MX_TIM4_Init();         /* PWM group 2: PD12-PD15 */
  MX_TIM5_Init();         /* PWM group 3: PH10-PH12, PI0 */
  MX_TIM8_Init();         /* PWM group 4: PI5-PI7, PI2 */
  MX_ADC1_Init();         /* Knob ADC */
  MX_CAN1_Init();         /* CAN1: UWB / motor bus */
  MX_CAN2_Init();         /* CAN2: UWB / motor bus */
  MX_SDIO_SD_Init();      /* SD card */
  MX_FATFS_Init();        /* FAT file system */
  MX_USB_DEVICE_Init();   /* USB CDC virtual COM */

  /* BSP initializations */
  led_off();
  mpu_device_init();
  init_quaternion();
  dbus_uart_init();
  can_filter_init(&hcan1);
  can_filter_init(&hcan2);
  oled_init();
  BSP_SD_Init();

  /* OLED splash screen */
  oled_clear(Pen_Clear);
  oled_LOGO();
  oled_refresh_gram();

  /* C620 motor control init */
  APP_C620_Init();

  /* Infinite loop */
  while (1)
  {
    APP_C620_Update();
    HAL_Delay(1);  /* 1ms loop for responsive button + CAN TX */
  }
}

/**
  * @brief System Clock Configuration
  *        HSE 12MHz -> PLL -> 168MHz SYSCLK
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void _Error_Handler(char *file, int line)
{
  while(1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) { }
#endif
