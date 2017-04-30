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
#ifndef BAROMETER_H_
#define BAROMETER_H_

#include "stm32f4xx_hal.h"
#include "spi.h"
#include "main.h"
#include <math.h>

/******************************************************************************/
/*                                Define                                      */
/******************************************************************************/
#define PROM_READ_COMMAND           0xA0 // Command to read the PROM
#define BAROMETER_RESET_COMMAND     0x1E // Command to reset the barometer on the SPI interface

#define CMD_ADC_256                 0x00 // ADC OSR=256
#define CMD_ADC_512                 0x02 // ADC OSR=512
#define CMD_ADC_1024                0x04 // ADC OSR=1024
#define CMD_ADC_2048                0x06 // ADC OSR=2056
#define CMD_ADC_4096                0x08 // ADC OSR=4096
#define CMD_ADC_CONV                0x40 // ADC conversion command
#define CMD_ADC_READ                0x00 // ADC read command
#define CMD_ADC_D1                  0x00 // ADC D1 conversion
#define CMD_ADC_D2                  0x10 // ADC D2 conversion

#define AIR_MOLAR_MASS            0.0289644 // kg par mol
#define GAZ_CONSTANT              8.31447

/******************************************************************************/
/*                             Type  Prototype                                */
/******************************************************************************/
typedef struct Barometer_s {
  float pressure;                    // Calibrated pressure
  float temperature;             // Calibrated temperature where 2000 = 20.00°C
  float Air_Density;            // air density value

  uint64_t baro_temp;                 // Container for temporary data

  uint32_t calibrationCoefficient[8];  // Coefficient given by the device in order to calibrate the measured values

  uint32_t D1;                        // Uncalibrated pressure
  uint32_t D2;                        // Uncalibrated temperature

  int64_t dT;                         // Temperature variation
  int64_t offset;                     // Offset to apply on the pressure value
  int64_t sensitivity;           // Sensitivity of the pressure pressure measure

  uint32_t isInitialized;

} Barometer_t;

/******************************************************************************/
/*                             Global variable                                */
/******************************************************************************/

/******************************************************************************/
/*                             Function prototype                             */
/******************************************************************************/

void Init_Barometer(Barometer_t * temp_barometer);
void Reset_Barometer(void);
uint16_t Read_Calibration_Coefficient(char coef_num);

uint32_t Read_Barometer_Data(char cmd);
void Compute_Barometer_Pressure(Barometer_t * this_baro);
void Compute_Air_Density(Barometer_t * temp_barometer);
float Convert_mBar_To_Altitude(float pressure);

#endif /* BAROMETER_H_ */
