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
#include "Altimeter.h"

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
void Init_Altimeter(Altimeter_t * temp_altimeter) {
  temp_altimeter->Barometric_Altitude = 0;
  temp_altimeter->LP_Filter_Altitude = 0;
  temp_altimeter->Filtered_Altitude = 0;
  temp_altimeter->Filtered_Acceleration = 0;
  temp_altimeter->Filtered_Velocity = 0;
  temp_altimeter->Initial_Altitude = 0;
  temp_altimeter->AGL_Altitude = 0;
  temp_altimeter->Apogee_Altitude = 0;
  temp_altimeter->Apogee_Estimation = 0;
  temp_altimeter->Burnout_Detected = 0;
  temp_altimeter->Burnout_Time = 0;
  temp_altimeter->Apogee_Detected = 0;
  temp_altimeter->Apogee_Time = 0;

  temp_altimeter->Apogee_Ejection_Delay = APOGEE_EJECTION_DELAY_MS;  //ms after apogee
  temp_altimeter->Ultrasonic_Delay = ULTRASONIC_DELAY_MS;   //ms after launch
  temp_altimeter->Flight_Altitude_trigger = FLIGHT_ALTITUDE_TRIGGER;  //metres
  temp_altimeter->Launch_Acceleration_trigger = LAUNCH_ACCEL_TRIGGER;
  temp_altimeter->Main_Ejection_Altitude = MAIN_EJECTION_ALTITUDE;

  temp_altimeter->Initial_Altitude_Count = 20;
  temp_altimeter->isInitialized = 1;
}

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
void Compute_Initial_Altitude(Altimeter_t * temp_altimeter) {
  temp_altimeter->Initial_Altitude = (temp_altimeter->Initial_Altitude + temp_altimeter->Barometric_Altitude) / 2;
}

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
void Apogee_Detection(Altimeter_t * temp_altimeter) {

  //estimation de l'apogee
  //n,est valide qu'en coasting ascent
  // s(apogee) = s0 + 1.5 * v^2 / a
  temp_altimeter->Apogee_Estimation = temp_altimeter->Filtered_Altitude
      + (temp_altimeter->Filtered_Velocity)
          * (temp_altimeter->Filtered_Velocity) * 1.5
          / temp_altimeter->Filtered_Acceleration;

  //essaie pour contré le sonic boom sans le delais
  //estime l'altitude de l'apogee et si la difference est trop grande
  //on annule le projet tu suite
  if (temp_altimeter->Filtered_Altitude - temp_altimeter->Apogee_Estimation
      < 500) {
    //test supplementaire, l'acceleration se doit d'etre negative...
    if (temp_altimeter->Filtered_Acceleration < 0) {
      //lorsque la vitesse est negative, cest l'apogee
      if (temp_altimeter->Filtered_Velocity < 0) {
        temp_altimeter->Apogee_Altitude = temp_altimeter->Filtered_Altitude;
        temp_altimeter->Apogee_Detected = 1;
      }
    }
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
void Burnout_Detection(Altimeter_t * temp_altimeter) {

  if (temp_altimeter->Filtered_Acceleration < 0) {
    temp_altimeter->Burnout_Detected = 1;
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
void Compute_AGL_Altitude(Altimeter_t * temp_altimeter) {
  temp_altimeter->AGL_Altitude = temp_altimeter->Barometric_Altitude
      - temp_altimeter->Initial_Altitude;
}

