/**
  ******************************************************************************
  * @file    app_uwb.c
  * @brief   UWB positioning application - CAN bus
  ******************************************************************************
  */
#include "app_uwb.h"
#include "can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern uwb_info_t uwb_data;

void APP_UWB_Init(void)
{
  can_filter_init(&hcan1);
  can_filter_init(&hcan2);
}

void APP_UWB_GetData(uwb_info_t *out)
{
  out->coor_x   = uwb_data.coor_x;
  out->corr_y   = uwb_data.corr_y;
  out->yaw      = uwb_data.yaw;
  out->sig_level = uwb_data.sig_level;
}
