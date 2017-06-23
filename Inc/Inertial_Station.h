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
#define CAN_ROCKET_STATE_REQ_ID   0x001 //sent by SGP
#define CAN_ROCKET_STATE_ID       0x011 //sent to SGP

#define CAN_ROCKET_ALTITUDE_REQ_ID   0x002 //sent by SGP
#define CAN_ROCKET_ALTITUDE_ID       0x012 //sent to SGP

#define CAN_GPS_LONGITUDE_ID      0x211
#define CAN_GPS_LATITUDE_ID       0x212
#define CAN_GPS_ALTITUDE_ID       0x213
#define CAN_GPS_FIX_TYPE_ID       0x214
#define CAN_GPS_N_SATELLITE_ID    0x215

#define CAN_GPS_HEADING_MOTION_ID   0x216
#define CAN_GPS_GROUND_SPEED_ID     0x217

#define CAN_ACCELERATION_X_ID 	  0x221
#define CAN_ACCELERATION_Y_ID     0x222
#define CAN_ACCELERATION_Z_ID     0x223

#define CAN_GYRO_YIELD_ID         0x231
#define CAN_GYRO_YAW_ID           0x232
#define CAN_GYRO_ROLL_ID          0x233

#define CAN_SGP_STATE             0x300
#define CAN_SGP_DESCENTTIME       0x301
#define CAN_SGP_HORZSPEED         0x302
#define CAN_SGP_VERTSPEED         0x303
#define CAN_SGP_POSTTRACKING      0x304

#define CAN_BEZIER_TIP_X          0x310
#define CAN_BEZIER_TIP_Y          0x311
#define CAN_BEZIER_FAP_X          0x312
#define CAN_BEZIER_FAP_Y          0x313
#define CAN_BEZIER_CP1_X          0x314
#define CAN_BEZIER_CP1_Y          0x315
#define CAN_BEZIER_CP2_X          0x316
#define CAN_BEZIER_CP2_Y          0x317

#define CAN_MOTORCMD_LEFT         0x400
#define CAN_MOTORCMD_RIGHT        0x401
#define CAN_MOTORPOS_LEFT         0x402
#define CAN_MOTORPOS_RIGHT        0x403


/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct Inertial_Station_s {
  float accel_x;
  float accel_y;
  float accel_z;

  float GPS_longitude;
  float GPS_latitude;
  uint32_t GPS_altitude; //gps gives it in mm

  uint32_t GPS_heading_of_motion; // gps gives it in degree multiplied by 10000
  uint32_t GPS_ground_speed; //gps gives it in mm per second

  uint32_t GPS_N_satellite;
  uint32_t GPS_Fix_Type;

  float gyro_roll;
  float gyro_yaw;
  float gyro_yield;

  float bezier_tip_x;
  float bezier_tip_y;
  float bezier_fap_x;
  float bezier_fap_y;
  float bezier_cp1_x;
  float bezier_cp1_y;
  float bezier_cp2_x;
  float bezier_cp2_y;

  uint32_t SGP_state;
  float SGP_descent_time;
  float SGP_horizontal_speed;
  float SGP_vertical_speed;
  float SGP_post_tracking;

  float motorcmd_left;
  float motorcmd_right;
  float motorpos_left;
  float motorpos_right;

  uint32_t canbus_comm_error;
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
