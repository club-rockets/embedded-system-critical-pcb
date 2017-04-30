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
#ifndef LED_H_
#define LED_H_

#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct LED_s {
  uint32_t Critical_LED1;  //led #1, at the interior of the board
  uint32_t Critical_LED2;  //led #2
  uint32_t Critical_LED3;  //led #3
  uint32_t Critical_LED4;  //led #4, at the exterior of the board

  uint32_t Status_LED_Proc_Good;		//led near switches
  uint32_t Status_LED_GPS_fixed;		//led near switches
  uint32_t Status_LED_Main_Armed;		//led near switches
  uint32_t Status_LED_Drogue_Armed;  //led near switches

  uint32_t isInitialized;  // Designate if the device has been initialized

} LED_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_LED(LED_t * temp_LED);
void Update_LED(LED_t * temp_LED);

#endif //LED_H_
