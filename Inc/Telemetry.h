/*********************************************************************************************
 ___                  _      ___   _____  ___
 |  _`\               ( )    (  _`\(_   _)(  _`\
 | (_) )   _      ___ | |/') | (_(_) | |  | (_(_)
 | ,  /  /'_`\  /'___)| , <  |  _)_  | |  `\__ \
 | |\ \ ( (_) )( (___ | |\`\ | (_( ) | |  ( )_) |
 (_) (_)`\___/'`\____)(_) (_)(____/' (_)  `\____)

 Copyright (c) 2016-2017
 All rights reserved

 RockETS, Montréal
 École de Technologies Supérieure
 *********************************************************************************************/

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <stdint.h>
#include <string.h>

#include "Message_Defs.h"

#include "stm32f4xx_hal.h"

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/

/******************************************************************************/
/*                              Type  Prototype                               */
/******************************************************************************/
typedef struct MessageQueueNode_t MessageQueueNode_t;
typedef struct Telemetry_t Telemetry_t;

struct MessageQueueNode_t {
	uint8_t* data;
	MessageQueueNode_t* next;
};

struct Telemetry_t {
	char Telemetry_ID[16];
	unsigned int Loop_Step;
	unsigned int Busy;
	uint8_t* rx;
	MessageQueueNode_t* queue;
	uint32_t isInitialized;
};

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/
void Init_Telemetry(Telemetry_t * temp_telemetry);

void send_data_message(Telemetry_t* telemetry, uint8_t messageid, ...);
void process_queue(Telemetry_t* telemetry);

#endif //TELEMETRY_H_
