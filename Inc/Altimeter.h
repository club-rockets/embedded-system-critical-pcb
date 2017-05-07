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
#ifndef ALTIMETER_H_
#define ALTIMETER_H_

#include "stm32f4xx_hal.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define APOGEE_EJECTION_DELAY_MS 	2000 //delais d'ejection apres la detection d'apogee en millisec
#define ULTRASONIC_DELAY_MS 		  1000 //delais minimum apres le launch avant de pouvoir detection l'apogee en millisec
#define LAUNCH_ACCEL_TRIGGER		  5 // valeur proportionnelle a l'accel qui declanche le launch state
#define FLIGHT_ALTITUDE_TRIGGER		100 // altitude apres la detection du launch a laquelle on change d'etat en metre
#define MAIN_EJECTION_ALTITUDE		333 //altitude de deploiement du MAIN chute en metre

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Altimeter_s {
  float Initial_Altitude;    		// value on the pad in meter
  float Barometric_Altitude; 		// live value in meter
  float Filtered_Altitude;      // kalman filter estimated altitude
  float Filtered_Acceleration;  // kalman filter estimated velocity
  float Filtered_Velocity;      // kalman filter estimated acceleration
  float AGL_Altitude;           // above ground level altitude
  float LP_Filter_Altitude;     // altitude filtered with low pass filter 4th order

  float Flight_Altitude_trigger; 	//altitude at which the state switch to in flight
  float Launch_Acceleration_trigger; 	//altitude at which the state switch to in flight
  float Main_Ejection_Altitude;
  float Ultrasonic_Delay;       // delay after launch to prevent false trigger
  float Apogee_Ejection_Delay;  // post-apogee delay

  float Apogee_Altitude;    // measure of the apogee
  float Apogee_Estimation;  // estimation de l'apogee en real time, free fall body

  uint32_t Burnout_Detected;    // burnout du moteur detecté
  uint32_t Burnout_Time;     // mission time at which engine burnout is reached in ms

  uint32_t Apogee_Detected;     // apogee detecté
  uint32_t Apogee_Time;			    // mission time at which apogee is reached in ms

  uint32_t Initial_Altitude_Count;
  uint32_t isInitialized;
} Altimeter_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Altimeter(Altimeter_t * temp_altimeter);
void Compute_Initial_Altitude(Altimeter_t * temp_altimeter);
void Compute_AGL_Altitude(Altimeter_t * temp_altimeter);
void Apogee_Detection(Altimeter_t * temp_altimeter);
void Burnout_Detection(Altimeter_t * temp_altimeter);

#endif // ALTIMETER_H_
