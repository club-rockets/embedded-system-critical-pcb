/*********************************************************************************************
 ___                  _      ___   _____  ___
 |  _`\               ( )    (  _`\(_   _)(  _`\
 | (_) )   _      ___ | |/') | (_(_) | |  | (_(_)
 | ,  /  /'_`\  /'___)| , <  |  _)_  | |  `\__ \
 | |\ \ ( (_) )( (___ | |\`\ | (_( ) | |  ( )_) |
 (_) (_)`\___/'`\____)(_) (_)(____/' (_)  `\____)

 Copyright (c) 2016-2017
 All rights reserved

 RockETS, Montreal
 Ecole de Technologies Superieures
 *********************************************************************************************/
#include "LED.h"

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void Init_LED(LED_t * temp_LED) {
  temp_LED->Critical_LED1 = 0;
  temp_LED->Critical_LED2 = 0;
  temp_LED->Critical_LED3 = 0;
  temp_LED->Critical_LED4 = 0;
  temp_LED->Status_LED_Drogue_Armed = 0;
  temp_LED->Status_LED_GPS_fixed = 0;
  temp_LED->Status_LED_Main_Armed = 0;
  temp_LED->Status_LED_Proc_Good = 0;

  temp_LED->isInitialized = 1;
}

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void Update_LED(LED_t * temp_LED) {

  if (temp_LED->Critical_LED1 == 1) {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
  }

  if (temp_LED->Critical_LED2 == 1) {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  }
  if (temp_LED->Critical_LED3 == 1) {
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
  }

  if (temp_LED->Critical_LED4 == 1) {
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
  }

  if (temp_LED->Status_LED_Drogue_Armed == 1) {
    HAL_GPIO_WritePin(LED_DROGUE_ARMED_GPIO_Port, LED_DROGUE_ARMED_Pin,
                      GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED_DROGUE_ARMED_GPIO_Port, LED_DROGUE_ARMED_Pin,
                      GPIO_PIN_RESET);
  }

  if (temp_LED->Status_LED_GPS_fixed == 1) {
    HAL_GPIO_WritePin(LED_GPS_SYNC_GPIO_Port, LED_GPS_SYNC_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED_GPS_SYNC_GPIO_Port, LED_GPS_SYNC_Pin, GPIO_PIN_RESET);
  }
  if (temp_LED->Status_LED_Main_Armed == 1) {
    HAL_GPIO_WritePin(LED_MAIN_ARMED_GPIO_Port, LED_MAIN_ARMED_Pin,
                      GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED_MAIN_ARMED_GPIO_Port, LED_MAIN_ARMED_Pin,
                      GPIO_PIN_RESET);
  }

  if (temp_LED->Status_LED_Proc_Good == 1) {
    HAL_GPIO_WritePin(LED_PROC_GOOD_GPIO_Port, LED_PROC_GOOD_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LED_PROC_GOOD_GPIO_Port, LED_PROC_GOOD_Pin,
                      GPIO_PIN_RESET);
  }
}

