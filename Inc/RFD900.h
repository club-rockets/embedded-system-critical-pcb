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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "main.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct RFD900_s {
  uint32_t Format;				// s reg #01
  uint32_t Serial_Speed;		// s reg #02
  uint32_t Air_Speed;			// s reg #03
  uint32_t Net_ID;				// s reg #04
  uint32_t TX_Power;			// s reg #05
  uint32_t ECC;				// s reg #06
  uint32_t Mavlink;			// s reg #07
  uint32_t Op_Resend;			// s reg #08
  uint32_t Min_Frequency;		// s reg #09
  uint32_t Max_Frequency;		// s reg #10
  uint32_t Number_of_Channels;  // s reg #11
  uint32_t Duty_Cycle;			// s reg #12
  uint32_t LBT_RSSI;			// s reg #13
  uint32_t Manchester;			// s reg #14
  uint32_t RTSCTS;				// s reg #15
  uint32_t Node_ID;			// s reg #16
  uint32_t Node_Destination;  // s reg #17
  uint32_t Sync_Any;			// s reg #18
  uint32_t Node_Count;			// s reg #19

  uint8_t RX[20];
  volatile uint32_t Wait_for_OK_IT;		// if interrupt is used to wait for OK
  uint32_t isInitialized;      // Designate if the device has been initialized

} RFD900_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void RFD900_reset_factory(void);
void RFD900_reboot(void);

void Init_RFD900(RFD900_t * RF_modem);
void RFD900_read_setting(RFD900_t * RF_modem);
void RFD900_write_TX_Power(RFD900_t * RF_modem);

