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

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "stm32f4xx_hal.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Telemetry_s {
  char   Telemetry_ID[16];
  unsigned int Loop_Step;
  unsigned int Busy;
  char   Rocket_State_String[64];
  char   Time_String_Telemetry[32];
  char   RX_JSON_string[64];
  char * TX_JSON_string;//must be char for JSON to work properly, does mem leak
  char * TX_JSON_Base_Station; //temp pour rajouter \n
  uint32_t isInitialized;
} Telemetry_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Telemetry(Telemetry_t * temp_telemetry);

#endif //TELEMETRY_H_
