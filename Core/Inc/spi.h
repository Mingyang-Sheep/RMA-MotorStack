/**
  ******************************************************************************
  * File Name          : SPI.h
  ******************************************************************************
  */
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi5;

void MX_SPI1_Init(void);
void MX_SPI5_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */
