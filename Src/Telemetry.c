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
#include "Telemetry.h"

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void Init_Telemetry(Telemetry_t * temp_telemetry) {
  temp_telemetry->Telemetry_ID = 1;
  temp_telemetry->Mission_Time = 0;

  temp_telemetry->Barometer_Altitude = 0;
  temp_telemetry->Barometer_AGL_Altitude = 0;
  temp_telemetry->Barometer_Temperature = 0;

  temp_telemetry->Estimated_AGL_Altitude = 0;
  temp_telemetry->Estimated_Acceleration = 0;
  temp_telemetry->Estimated_Velocity = 0;

  temp_telemetry->isInitialized = 1;
}



