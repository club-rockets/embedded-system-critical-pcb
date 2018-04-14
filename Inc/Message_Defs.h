/*
 * message.h
 *
 *  Created on: Feb 19, 2018
 *      Author: ngc7293
 */

#ifndef MESSAGE_DEFS_H_
#define MESSAGE_DEFS_H_

// clang-format off
#define MSG_ID_EJECTION_SETTINGS 30
#define MSG_ID_ALTIMETER_DATA	 40
#define MSG_ID_ACCELERATION      42
#define MSG_ID_GYRO              43
#define MSG_ID_ROCKET_STATE      50
#define MSG_ID_PARACHUTE_STATE   51
#define MSG_ID_GPS_METADATA      60
#define MSG_ID_GPS_DATA          61

// f : float
// b : byte (uint8_t)
// i : long (uint32_t)
#define MSG_FORMAT_EJECTION_SETTINGS "fff"
#define MSG_FORMAT_ALTIMETER_DATA    "ffff"
#define MSG_FORMAT_ACCELERATION      "fff"
#define MSG_FORMAT_GYRO              "fff"
#define MSG_FORMAT_ROCKET_STATE      "b"
#define MSG_FORMAT_PARACHUTE_STATE   "bb"
#define MSG_FORMAT_GPS_METADATA      "bb"
#define MSG_FORMAT_GPS_DATA	         "ffi"
// clang-format on

#endif
