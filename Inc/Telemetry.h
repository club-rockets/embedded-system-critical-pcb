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
  uint32_t Telemetry_ID;
  float Barometer_Temperature;
  float Barometer_Pressure;
  float Barometer_Altitude;
  float Barometer_AGL_Altitude;
  float Mission_Time;
  float Estimated_AGL_Altitude;
  float Estimated_Velocity;
  float Estimated_Acceleration;

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
