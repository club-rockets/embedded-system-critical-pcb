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
#include "Drogue_Parachute.h"

void init_Drogue_Parachute(Drogue_Parachute_t * temp_parachute) {
  temp_parachute->Estimated_Altitude_Deployed = 0;
  temp_parachute->Barometric_Altitude_Deployed = 0;
  temp_parachute->Ejection_Charge_Detect = 0;
  temp_parachute->Ejection_Charge_Fire = 0;
  temp_parachute->Ejection_Toggle_Number = 2 * DROGUE_EJECTION_TOGGLE_NUMBER;

  temp_parachute->isInitialized = 1;
}

void Update_Drogue_Continuity(Drogue_Parachute_t * temp_parachute) {
  if (!HAL_GPIO_ReadPin(SENSE_DROGUE_GPIO_Port, SENSE_DROGUE_Pin)) {
    temp_parachute->Ejection_Charge_Detect = 1;
  } else {
    temp_parachute->Ejection_Charge_Detect = 0;
  }
}

void Update_Drogue_Fire(Drogue_Parachute_t * temp_parachute) {
  if (temp_parachute->Ejection_Charge_Fire == 1) {
    if (temp_parachute->Ejection_Toggle_Number > 0) {
      HAL_GPIO_TogglePin(FIRE_DROGUE_GPIO_Port, FIRE_DROGUE_Pin);
      temp_parachute->Ejection_Toggle_Number -= 1;
    } else {
      HAL_GPIO_WritePin(FIRE_DROGUE_GPIO_Port, FIRE_DROGUE_Pin, GPIO_PIN_RESET);
    }
  } else {
    temp_parachute->Ejection_Toggle_Number = DROGUE_EJECTION_TOGGLE_NUMBER;
    HAL_GPIO_WritePin(FIRE_DROGUE_GPIO_Port, FIRE_DROGUE_Pin, GPIO_PIN_RESET);
  }
}
