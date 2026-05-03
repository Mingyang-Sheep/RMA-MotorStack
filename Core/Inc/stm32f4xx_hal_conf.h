/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_conf.h
  * Description        : HAL configuration file for RMA-MotorStack
  ******************************************************************************
  */
#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ########################## Module Selection ############################## */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CAN_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED

/* ########################## HSE/HSI Values ############################### */
#if !defined(HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)12000000U)
#endif
#if !defined(HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    ((uint32_t)100U)
#endif
#if !defined(HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000U)
#endif
#if !defined(LSI_VALUE)
 #define LSI_VALUE  ((uint32_t)32000U)
#endif
#if !defined(LSE_VALUE)
 #define LSE_VALUE  ((uint32_t)32768U)
#endif
#if !defined(LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    ((uint32_t)5000U)
#endif
#if !defined(EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    ((uint32_t)12288000U)
#endif

/* ########################### System Configuration ######################### */
#define  VDD_VALUE                ((uint32_t)3300U)
#define  TICK_INT_PRIORITY        ((uint32_t)0U)
#define  USE_RTOS                 0U
#define  PREFETCH_ENABLE          1U
#define  INSTRUCTION_CACHE_ENABLE 1U
#define  DATA_CACHE_ENABLE        1U

/* ################## Ethernet configuration ############################### */
#define MAC_ADDR0   2U
#define MAC_ADDR1   0U
#define MAC_ADDR2   0U
#define MAC_ADDR3   0U
#define MAC_ADDR4   0U
#define MAC_ADDR5   0U
#define ETH_RX_BUF_SIZE  ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE  ETH_MAX_PACKET_SIZE
#define ETH_RXBUFNB      ((uint32_t)4U)
#define ETH_TXBUFNB      ((uint32_t)4U)
#define DP83848_PHY_ADDRESS       0x01U
#define PHY_RESET_DELAY           ((uint32_t)0x000000FFU)
#define PHY_CONFIG_DELAY          ((uint32_t)0x00000FFFU)
#define PHY_READ_TO               ((uint32_t)0x0000FFFFU)
#define PHY_WRITE_TO              ((uint32_t)0x0000FFFFU)
#define PHY_BCR                   ((uint16_t)0x0000U)
#define PHY_BSR                   ((uint16_t)0x0001U)
#define PHY_RESET                 ((uint16_t)0x8000U)
#define PHY_LOOPBACK              ((uint16_t)0x4000U)
#define PHY_FULLDUPLEX_100M       ((uint16_t)0x2100U)
#define PHY_HALFDUPLEX_100M       ((uint16_t)0x2000U)
#define PHY_FULLDUPLEX_10M        ((uint16_t)0x0100U)
#define PHY_HALFDUPLEX_10M        ((uint16_t)0x0000U)
#define PHY_AUTONEGOTIATION       ((uint16_t)0x1000U)
#define PHY_RESTART_AUTONEGOTIATION ((uint16_t)0x0200U)
#define PHY_POWERDOWN             ((uint16_t)0x0800U)
#define PHY_ISOLATE               ((uint16_t)0x0400U)
#define PHY_AUTONEGO_COMPLETE     ((uint16_t)0x0020U)
#define PHY_LINKED_STATUS         ((uint16_t)0x0004U)
#define PHY_JABBER_DETECTION      ((uint16_t)0x0002U)
#define PHY_SR                    ((uint16_t)0x10U)
#define PHY_SPEED_STATUS          ((uint16_t)0x0002U)
#define PHY_DUPLEX_STATUS         ((uint16_t)0x0004U)

/* ################## SPI peripheral configuration ########################## */
#define USE_SPI_CRC              0U

/* Includes ------------------------------------------------------------------*/
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f4xx_hal_rcc.h"
#endif
#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f4xx_hal_gpio.h"
#endif
#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f4xx_hal_dma.h"
#endif
#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f4xx_hal_cortex.h"
#endif
#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32f4xx_hal_adc.h"
#endif
#ifdef HAL_CAN_MODULE_ENABLED
  #include "stm32f4xx_hal_can.h"
#endif
#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f4xx_hal_flash.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f4xx_hal_pwr.h"
#endif
#ifdef HAL_SD_MODULE_ENABLED
  #include "stm32f4xx_hal_sd.h"
#endif
#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32f4xx_hal_spi.h"
#endif
#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f4xx_hal_tim.h"
#endif
#ifdef HAL_UART_MODULE_ENABLED
  #include "stm32f4xx_hal_uart.h"
#endif
#ifdef HAL_USART_MODULE_ENABLED
  #include "stm32f4xx_hal_usart.h"
#endif
#ifdef HAL_PCD_MODULE_ENABLED
  #include "stm32f4xx_hal_pcd.h"
#endif

/* ########################## Assert Selection ############################## */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
