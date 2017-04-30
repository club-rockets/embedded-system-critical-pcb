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
#ifndef DROGUE_PARACHUTE_H_
#define DROGUE_PARACHUTE_H_

#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define DROGUE_EJECTION_TOGGLE_NUMBER	10 //nombre de pulses de 50ms on shoot sur la e-match

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Drogue_Parachute_s {
  float Estimated_Altitude_Deployed;  // altitude estimé du deploiement
  float Barometric_Altitude_Deployed;  // altitude mesuré du deploiement
  uint32_t Ejection_Charge_Detect;	  // if the ejection system is armed
  uint32_t Ejection_Charge_Fire;		  // if the ejection system is armed
  uint32_t Ejection_Toggle_Number;	  // number of impulsion in the square wave

  uint32_t isInitialized;

} Drogue_Parachute_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Drogue_Parachute(Drogue_Parachute_t * temp_parachute);
void Update_Drogue_Continuity(Drogue_Parachute_t * temp_parachute);
void Update_Drogue_Fire(Drogue_Parachute_t * temp_parachute);

#endif /* DROGUE_PARACHUTE_H_ */
