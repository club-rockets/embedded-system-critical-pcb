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
#include "Buzzer.h"

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
void Init_Buzzer(Buzzer_t * temp_buzzer) {
  temp_buzzer->Buzzer_enable = 0;
  temp_buzzer->Buzzer_mode = 0;
  temp_buzzer->isInitialized = 1;
}

/*********************************************************************************************
 * Update_Alarm_Rocket
 * generate buzzer sound patern by counting the number of re-entry (each is 50ms)
 *
 * mode = 1:
 * _|¯|______________
 *
 * mode = 2:
 * _|¯|_|¯|__________
 *
 * mode = 3:
 * _|¯|_|¯|_|¯|______
 *
 * ARGUMENTS : Buzzer_t
 *    -
 *
 * RETURN : void
 *    -
 *
 *********************************************************************************************/
void Update_Alarm_Rocket(Buzzer_t * temp_buzzer) {
  static uint32_t i;  //counter for every loop entry

  if (temp_buzzer->Buzzer_enable) {
    switch (i) {
      case 1:
        if (temp_buzzer->Buzzer_mode >= 1) {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
        } else {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        }

        break;

      case 2:
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        break;

      case 4:
        if (temp_buzzer->Buzzer_mode >= 2) {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
        } else {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        }
        break;

      case 6:
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        break;

      case 7:
        if (temp_buzzer->Buzzer_mode >= 3) {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
        } else {
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        }
        break;

      case 9:
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        break;

      case 20:
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
        i = 0;
        break;
      default:
        i = 0;
        break;
    }

  } else {
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
  }

  i++;

}
