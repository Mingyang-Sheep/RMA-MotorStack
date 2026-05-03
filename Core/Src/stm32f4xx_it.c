/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Unified interrupt service routines for RMA-MotorStack
  ******************************************************************************
  */
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "usart.h"

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern SD_HandleTypeDef hsd;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
void uart_receive_handler(UART_HandleTypeDef *huart);

/******************************************************************************/
/*            Cortex-M4 Processor Exception Handlers                          */
/******************************************************************************/

void NMI_Handler(void) { }
void HardFault_Handler(void) { while(1) { } }
void MemManage_Handler(void) { while(1) { } }
void BusFault_Handler(void) { while(1) { } }
void UsageFault_Handler(void) { while(1) { } }
void SVC_Handler(void) { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void) { }

void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/* USART1 (DBUS remote control) */
void USART1_IRQHandler(void)
{
  uart_receive_handler(&huart1);
  HAL_UART_IRQHandler(&huart1);
}

/* USART6 (debug serial) */
void USART6_IRQHandler(void)
{
  HAL_UART_IDLE_IRQHandler(&huart6);
  HAL_UART_IRQHandler(&huart6);
}

/* DMA2 Stream2 - USART1_RX */
void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

/* DMA2 Stream1 - USART6_RX (USB VCP) */
void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_rx);
}

/* DMA2 Stream6 - USART6_TX (USB VCP) */
void DMA2_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_tx);
}

/* SDIO */
void SDIO_IRQHandler(void)
{
  HAL_SD_IRQHandler(&hsd);
}

/* USB OTG FS */
void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

/* CAN1 */
void CAN1_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}
void CAN1_RX1_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}

/* CAN2 */
void CAN2_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}
void CAN2_RX1_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}

/* Weak callbacks required by HAL but not used */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc) { }
