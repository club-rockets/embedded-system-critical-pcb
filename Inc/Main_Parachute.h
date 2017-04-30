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
#ifndef MAIN_PARACHUTE_H_
#define MAIN_PARACHUTE_H_

#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define MAIN_EJECTION_TOGGLE_NUMBER	10

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Main_Parachute_s {
  float Estimated_Altitude_Deployed;
  float Barometric_Altitude_Deployed;
  uint32_t Ejection_Charge_Detect;	// if the ejection system is armed
  uint32_t Ejection_Charge_Fire;		// if the ejection system is armed
  uint32_t Ejection_Toggle_Number;	// number of impulsion in the square wave

  uint32_t isInitialized;

} Main_Parachute_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Main_Parachute(Main_Parachute_t * temp_parachute);
void Update_Main_Continuity(Main_Parachute_t * temp_parachute);
void Update_Main_Fire(Main_Parachute_t * temp_parachute);

#endif /* MAIN_PARACHUTE_H_ */
