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
#include "Main_Parachute.h"

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
void init_Main_Parachute(Main_Parachute_t * temp_parachute) {
  temp_parachute->Estimated_Altitude_Deployed = 0;
  temp_parachute->Barometric_Altitude_Deployed = 0;
  temp_parachute->Ejection_Charge_Detect = 0;
  temp_parachute->Ejection_Charge_Fire = 0;
  temp_parachute->Ejection_Toggle_Number = 2 * MAIN_EJECTION_TOGGLE_NUMBER;

  temp_parachute->isInitialized = 1;
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
void Update_Main_Continuity(Main_Parachute_t * temp_parachute) {
  if (!HAL_GPIO_ReadPin(SENSE_MAIN_GPIO_Port, SENSE_MAIN_Pin)) {
    temp_parachute->Ejection_Charge_Detect = 1;
  } else {
    temp_parachute->Ejection_Charge_Detect = 0;
  }
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
void Update_Main_Fire(Main_Parachute_t * temp_parachute) {
  if (temp_parachute->Ejection_Charge_Fire == 1) {
    if (temp_parachute->Ejection_Toggle_Number > 0) {
      HAL_GPIO_TogglePin(FIRE_MAIN_GPIO_Port, FIRE_MAIN_Pin);
      temp_parachute->Ejection_Toggle_Number -= 1;
    } else {
      HAL_GPIO_WritePin(FIRE_MAIN_GPIO_Port, FIRE_MAIN_Pin, GPIO_PIN_RESET);
    }
  } else {
    HAL_GPIO_WritePin(FIRE_MAIN_GPIO_Port, FIRE_MAIN_Pin, GPIO_PIN_RESET);
    temp_parachute->Ejection_Toggle_Number = 2 * MAIN_EJECTION_TOGGLE_NUMBER;
  }
}
