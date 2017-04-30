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
#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Buzzer_s {
  uint32_t Buzzer_mode;   //nombre de bip par cycle: 0,1,2,3
  uint32_t Buzzer_enable;  //si la sirene est active

  uint32_t isInitialized;
} Buzzer_t;
/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Buzzer(Buzzer_t * temp_buzzer);
void Update_Alarm_Rocket(Buzzer_t * temp_buzzer);

#endif //BUZZER_H_
