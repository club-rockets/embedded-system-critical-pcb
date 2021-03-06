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
#include "Inertial_Station.h"

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
void Init_Inertial_Station(Inertial_Station_t * temp_inertial_station) {
  temp_inertial_station->gyro_q1 = 0;
  temp_inertial_station->gyro_q2 = 0;
  temp_inertial_station->gyro_q3 = 0;
  temp_inertial_station->gyro_q4 = 0;

  temp_inertial_station->accel_x = 0;
  temp_inertial_station->accel_y = 0;
  temp_inertial_station->accel_z = 0;

  temp_inertial_station->GPS_altitude = 0;
  temp_inertial_station->GPS_latitude = 0;
  temp_inertial_station->GPS_longitude = 0;

  temp_inertial_station->GPS_Fix_Type = 0;
  temp_inertial_station->GPS_N_satellite = 0;

  temp_inertial_station->canbus_comm_error = 0;
  temp_inertial_station->IsInitialized = 1;
}

