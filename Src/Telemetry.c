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
  strcpy(temp_telemetry->Telemetry_ID, "Amarok");
  temp_telemetry->TX_JSON_string = NULL;
  temp_telemetry->Busy = 0;
  temp_telemetry->Loop_Step = 20; //1seconde
  temp_telemetry->isInitialized = 1;
}



