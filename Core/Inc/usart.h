/**
  ******************************************************************************
  * File Name          : USART.h
  ******************************************************************************
  */
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

void MX_USART1_UART_Init(void);
void MX_USART6_UART_Init(void);
void HAL_UART_Receive_IT_IDLE(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void HAL_UART_IDLE_IRQHandler(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */
