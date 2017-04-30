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

#ifndef KALMAN_H_
#define KALMAN_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "arm_matrix.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define ALTITUDE_SIGMA 		2.0	 // ecart type de la mesure d'altitude barometrique
#define ACCELERATION_SIGMA 	0  // ecart type de la mesure d'acceleration

#define BAROMETER_H_GAIN		  1	//1 ou 0, maps le capteur dans le filtre
#define ACCELEROMETER_H_GAIN	0 //1 ou 0, maps le capteur dans le filtre

#define KALMAN_TIME_STEP		0.05        // pas de calcul en seconde
#define CONVERGENCE_TIMEOUT		50        // nombre d'essai pour trouver les gains de kalman optimals
#define CONVERGENCE_PRECISION	0.000001  //critere de convergence

#define VAR_MODEL_S 10.4  // variance de l'altitude dans le modele
#define VAR_MODEL_V 5.6   // variance de la vitesse dans le modele
#define VAR_MODEL_A 11.2 // variance de l'acceleration dans le modele
/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Kalman_s {
  float Acceleration;
  float Barometric_Altitude;

  float Filtered_Altitude;
  float Filtered_Velocity;
  float Filtered_Acceleration;

  uint32_t Filter_Converge;
  uint32_t isInitialized;

} Kalman_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Kalman(Kalman_t * temp_kalman);
void Update_Kalman(Kalman_t * temp_kalman);

#endif //KALMAN_H_
