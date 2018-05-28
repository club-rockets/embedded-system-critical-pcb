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
#include "Telemetry.h"
#include "Message_Defs.h"

#include "usart.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*                        "Private" function prototypes                       */
/******************************************************************************/
/* Message functions */
char* _message_format(uint8_t messageid);
uint8_t _param_count(char* format);
uint8_t _compute_crc(uint8_t* message, uint8_t length);

/* Shortcuts */
int _memcpy_f(void* dest, float f);
int _memcpy_i(void* dest, uint32_t i);
int _memcpy_b(void* dest, uint8_t b);

/* Queue functions */
void _push_back(Telemetry_t* telemetry, uint8_t* message);
uint8_t* _pop_front(Telemetry_t* telemetry);

/******************************************************************************/
/*                                  Functions                                 */
/******************************************************************************/
void Init_Telemetry(Telemetry_t* temp_telemetry)
{
    strcpy(temp_telemetry->Telemetry_ID, "Amarok");
    temp_telemetry->Busy = 0;
    temp_telemetry->Loop_Step = 20; // 1seconde
    temp_telemetry->Alt_Loop_Step = 20; // 1seconde
    temp_telemetry->isInitialized = 1;
    temp_telemetry->queue = NULL;
}

void send_data_message(Telemetry_t* telemetry, uint8_t messageid, ...)
{
    /* declare */
    uint8_t i, n_args;
    va_list args;
    char* format;

    uint8_t* message;
    uint8_t length;

    message = malloc(256);

    /* header */
    message[0] = 0xFA; // preamble
    message[1] = 0x00; // source
    message[2] = 0xFF; // destination
    message[4] = messageid;
    length = 5;

    /* payload */
    format = _message_format(messageid);
    n_args = _param_count(format);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvarargs"
    va_start(args, n_args);
#pragma GCC diagnostic pop

    for (i = 0; i < n_args; ++i) {
        switch (format[i]) {
        case 'f':
            length += _memcpy_f(&message[length], va_arg(args, double));
            break;
        case 'i':
            length += _memcpy_i(&message[length], va_arg(args, int));
            break;
        case 'b':
            length += _memcpy_b(&message[length], va_arg(args, int));
            break;
        }
    }

    /* add crc and resize */
    message[3] = length + 1;
    message[length] = _compute_crc(message, length);
    length += 1;
    message = (uint8_t*)realloc(message, length);

    _push_back(telemetry, message);

    if (!telemetry->Busy) {
        telemetry->Busy = 1;
        HAL_UART_Transmit_DMA(&huart2, message,
            message[3]);
    }

    //free(message);
    /* cleanup */
    va_end(args);
}

void process_queue(Telemetry_t* telemetry)
{
    free(_pop_front(telemetry));

    if (telemetry->queue) {
        HAL_UART_Transmit_DMA(&huart2, telemetry->queue->data,
            telemetry->queue->data[3]);
    } else {
        telemetry->Busy = 0;
    }
}

char* _message_format(uint8_t messageid)
{
    switch (messageid) {
    case MSG_ID_EJECTION_SETTINGS:
        return MSG_FORMAT_EJECTION_SETTINGS;
    case MSG_ID_ALTIMETER_DATA:
        return MSG_FORMAT_ALTIMETER_DATA;
    case MSG_ID_ACCELERATION:
        return MSG_FORMAT_ACCELERATION;
    case MSG_ID_GYRO:
        return MSG_FORMAT_GYRO;
    case MSG_ID_ROCKET_STATE:
        return MSG_FORMAT_ROCKET_STATE;
    case MSG_ID_PARACHUTE_STATE:
        return MSG_FORMAT_PARACHUTE_STATE;
    case MSG_ID_GPS_METADATA:
        return MSG_FORMAT_GPS_METADATA;
    case MSG_ID_GPS_DATA:
        return MSG_FORMAT_GPS_DATA;
    case 0xF0:
    	return "f";
    }
    return "";
}

uint8_t _param_count(char* format) { return strlen(format); }

uint8_t _compute_crc(uint8_t* message, uint8_t length)
{
    uint8_t sum = 0;

    for (uint8_t i = 0; i < length; i++)
        sum += message[i];

    return ((uint8_t)~sum) + 1;
}

int _memcpy_f(void* dest, float f)
{
    memcpy(dest, &f, sizeof(float));
    return sizeof(float);
}

int _memcpy_i(void* dest, uint32_t i)
{
    memcpy(dest, &i, sizeof(uint32_t));
    return sizeof(uint32_t);
}

int _memcpy_b(void* dest, uint8_t b)
{
    memcpy(dest, &b, sizeof(uint8_t));
    return sizeof(uint8_t);
}

void _push_back(Telemetry_t* telemetry, uint8_t* message)
{
    MessageQueueNode_t* current = telemetry->queue;

    if (!current) {
        telemetry->queue = malloc(sizeof(MessageQueueNode_t));
        telemetry->queue->data = message;
        telemetry->queue->next = NULL;
        return;
    }

    while (current->next) {
        current = current->next;
    }
    current->next = malloc(sizeof(MessageQueueNode_t));
    current->next->data = message;
    current->next->next = NULL;
}

uint8_t* _pop_front(Telemetry_t* telemetry)
{
    uint8_t* data;
    MessageQueueNode_t* front;

    if (!telemetry->queue) {
        return NULL;
    }

    front = telemetry->queue;
    telemetry->queue = front->next;

    data = front->data;
    free(front);

    return data;
}
