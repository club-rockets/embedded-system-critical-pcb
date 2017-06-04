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

#ifndef MAIN_GEMMA_H_
#define MAIN_GEMMA_H_

#include "string.h"
#include "math.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

#include "Pressure_data.h"

#include "Barometer.h"
#include "Buzzer.h"
#include "RFD900.h"
#include "LED.h"
#include "Main_Parachute.h"
#include "Drogue_Parachute.h"
#include "Altimeter.h"
#include "Telemetry.h"
#include "Inertial_Station.h"
#include "Kalman.h"
#include "Low_Pass_Filter.h"

#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "can.h"
#include "usb_device.h"
#include "cJSON.h"
#include "User_Setting_Backup.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define FILENAME "RockETS.csv"
#define DATA_LOG_COL_NAME "RTC Time,Rocket State,Mission Time (ms), Main Loop Use (%),Main Detect,Main Fire,Drogue Detect,Drogue Fire,Barometer Temperature (degC),Air Density (Kg/m^3),Speed of Sound (m/s),Barometer Altitude (m),AGL Altitude (m),Estimated Altitude (m),Estimated Vertical Speed (m/s),Estimated Vertical Acceleration (m/s/s)\n"
#define DATA_LOG_HEADER "\nRockETS Data Logger Version 2.0\n"

#define MAIN_LOOP_TIME 50 //period du timer principale en ms
/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef enum Rocket_State_m {
  INITIALISATION = 0,
  STANDBY_ON_PAD,
  LAUNCH,
  POWERED_ASCENT,
  ENGINE_BURNOUT,
  COASTING_ASCENT,
  APOGEE_REACHED,
  DROGUE_DEPLOYMENT,
  DROGUE_DESCENT,
  MAIN_DEPLOYMENT,
  MAIN_DESCENT,
  LANDING,
  RECOVERY,
  PICKEDUP
} Rocket_State_t;

typedef struct Rocket_s {
  Rocket_State_t Rocket_State;
  uint32_t Main_Compute_Time;		//in ms
  uint32_t Main_Loop_Time_Step;  //in ms
  uint32_t Mission_Time;			//in ms

  Buzzer_t * Buzzer;
  LED_t * LED;
  Barometer_t * Barometer;
  Altimeter_t * Altimeter;
  Telemetry_t * Telemetry;
  Inertial_Station_t * Inertial_Station;
  RFD900_t * RFD900;

  Kalman_t * Kalman_Altitude;

  Main_Parachute_t * Main_Parachute;
  Drogue_Parachute_t * Drogue_Parachute;

  uint32_t isInitialized;

} Rockets_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/
Rockets_t RocketsVar = { .Rocket_State = INITIALISATION };

//RTC time and date handler
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
RTC_AlarmTypeDef sAlarm;

//FATFS parameters
FATFS fs;         // Work area (file system object) for logical drive
FIL data_file;  // file objects for data logging
FIL error_file;  // file objects for error logging
FRESULT res;        // FatFs function common result code
uint32_t byteread, bytewritten;         // File R/W count

// buffer pour sauvegarder des donnees
uint8_t Save_String[512] = { 0 };
uint8_t Rocket_State_String[64] = { 0 };

// buffer pour comm CANbus
CanTxMsgTypeDef CanTx_msg;
CanRxMsgTypeDef CanRx_msg;
CAN_FilterConfTypeDef CAN_FilterStruct;

//buffer pour le virtual com port USB
uint8_t USB_CDC_RX[64] = { 0 };
uint8_t USB_CDC_TX[64] = { 0 };

//telemetry
char float_buffer[24] = {0};


// devices global variable
Backup_Setting_t Backup_Settings;

Altimeter_t Altimeter;
Barometer_t Barometer;
Buzzer_t Buzzer;
Drogue_Parachute_t Drogue_Parachute;
Inertial_Station_t Inertial_Station;
Kalman_t Kalman_Altitude = { .Barometric_Altitude = 0, .Acceleration = 0 };
LED_t LED;
Main_Parachute_t Main_Parachute;
Telemetry_t Telemetry;
RFD900_t RFD900 = { .RX = { 0 } };
LP_Filter_t LP_Filter;



/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_rocket(Rockets_t * temp_rocket);
void State_Manager(Rockets_t * temp_rocket);
void separateDecimalValue(float_t value, int16_t * buff);
void Get_State_String(Rockets_t * temp_rocket, uint8_t * buff);
void ftos(float input, char * output);

#endif /* MAIN_GEMMA_H_ */

