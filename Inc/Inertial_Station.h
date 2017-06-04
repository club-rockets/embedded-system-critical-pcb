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

#ifndef INERTIAL_STATION_H_
#define INERTIAL_STATION_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define CAN_ROCKET_STATE_REQ_ID   0x001
#define CAN_ROCKET_STATE_ID       0x011

#define CAN_GPS_LONGITUDE_ID      0x211
#define CAN_GPS_LATITUDE_ID       0x212
#define CAN_GPS_ALTITUDE_ID       0x213
#define CAN_GPS_FIX_TYPE_ID       0x214
#define CAN_GPS_N_SATELLITE_ID    0x215

#define CAN_ACCELERATION_X_ID 	  0x221
#define CAN_ACCELERATION_Y_ID     0x222
#define CAN_ACCELERATION_Z_ID     0x223

#define CAN_GYRO_PITCH_ID         0x221
#define CAN_GYRO_YAW_ID           0x222
#define CAN_GYRO_ROLL_ID          0x223



/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Inertial_Station_s {
  float accel_x;
  float accel_y;
  float accel_z;

  float GPS_longitude;
  float GPS_latitude;
  float GPS_altitude;

  uint32_t GPS_N_satellite;
  uint32_t Fix_Type;

  float gyro_roll;
  float gyro_yaw;
  float gyro_yield;

  uint32_t IsInitialized;

} Inertial_Station_t;
/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Inertial_Station(Inertial_Station_t * temp_inertial_station);

#endif //INERTIAL_STATION_H_
