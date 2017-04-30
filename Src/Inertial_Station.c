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
  temp_inertial_station->roll_rate = 0;
  temp_inertial_station->yaw = 0;
  temp_inertial_station->yeild = 0;

  temp_inertial_station->x_acceleration = 0;
  temp_inertial_station->y_acceleration = 0;
  temp_inertial_station->z_acceleration = 0;

  temp_inertial_station->GPS_altitude = 0;
  temp_inertial_station->GPS_latitude = 0;
  temp_inertial_station->GPS_longitude = 0;

  temp_inertial_station->GPS_locked = 0;
  temp_inertial_station->Fix_Type = 0;
  temp_inertial_station->GPS_N_satellite = 0;

  temp_inertial_station->Comm_Is_Up = 0;
  temp_inertial_station->IsInitialized = 1;
}

