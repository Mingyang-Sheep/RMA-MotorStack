/**
  ******************************************************************************
  * File Name          : SDIO.h
  ******************************************************************************
  */
#ifndef __sdio_H
#define __sdio_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SD_HandleTypeDef hsd;

void MX_SDIO_SD_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ sdio_H */
