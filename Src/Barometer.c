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
#include "Barometer.h"

/*********************************************************************************************
 * Reads the calibration coefficients from the barometer (MS5607) device.
 *
 * ARGUMENTS :
 * 		- coef_num => Coefficient number index to be received.
 *
 * RETURN :
 * 		- The coefficient number corresponding to the index that was passed as
 * 		argument.
 *********************************************************************************************/
uint16_t Read_Calibration_Coefficient(char coef_num) {
  unsigned int rC = 0;

  uint8_t tx[3] = { ((uint8_t) (PROM_READ_COMMAND + coef_num * 2)), 0x00, 0x00 };
  uint8_t rx[3] = { 0 };

  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx, rx, (uint16_t) sizeof(tx), 1000);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

  rC = rx[1] << 8;
  rC = rC + rx[2];

  return rC;
}

/*********************************************************************************************
 * Sends the reset command to the barometer (MS5607)
 *
 * ARGUMENTS :
 * 		- void
 *
 * RETURN :
 * 		- void
 *********************************************************************************************/
void Reset_Barometer(void) {
  uint8_t tx[3] = { BAROMETER_RESET_COMMAND, 0x00, 0x00 };

  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, tx, 1, 10);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

  //must wait approximatively 2.8ms after reset
  HAL_Delay(3);
}

/*********************************************************************************************
 * Sends the ADC read command to the MS5607 device, which results in receiving
 * the raw pressure or raw temperature value.
 *
 * Pretty much copy pasted from Christian Roy's code from the BeagleBone Black
 * version.
 *
 * ARGUMENTS :
 * 		- cmd => Determines the timeout to be applied
 *
 * RETURN :
 * 		- void
 *********************************************************************************************/
uint32_t Read_Barometer_Data(char cmd) {
  uint8_t rx[10] = { 0 };
  uint8_t tx[10] = { 0 };

  //pull chip select low to initiate communication
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

  // send conversion command
  tx[0] = (CMD_ADC_CONV + cmd);
  HAL_SPI_Transmit(&hspi2, &tx[0], 1, 10);

  //wait necessary conversion time, which depends on the specific command
  switch (cmd & 0x0F) {
    case CMD_ADC_256:
      //delay = 900us
      HAL_Delay(1);
      break;
    case CMD_ADC_512:
      //delay = 3ms;
      HAL_Delay(3);
      break;
    case CMD_ADC_1024:
      //delay = 4ms;
      HAL_Delay(4);
      break;
    case CMD_ADC_2048:
      //delay = 6ms;
      HAL_Delay(6);
      break;
    case CMD_ADC_4096:
      //delay = 10ms;
      HAL_Delay(10);
      break;
  }

  // pull chip select high to finish the conversion
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
  /**************************************************************************/
  HAL_Delay(10);
  /**************************************************************************/
  // pull chip select low to start new command
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

  // Receive the data after the conversion
  HAL_SPI_Receive(&hspi2, &rx[0], 1, 10);
  HAL_SPI_Receive(&hspi2, &rx[1], 1, 10);
  HAL_SPI_Receive(&hspi2, &rx[2], 1, 10);
  HAL_SPI_Receive(&hspi2, &rx[3], 1, 10);

  // pull CS high after read
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

  // Somehow, the first byte received is garbage
  // return the 3 byte into a variable
  return (rx[1] << 16) | (rx[2] << 8) | (rx[3]);
}

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void Compute_Barometer_Pressure(Barometer_t * temp_barometer) {
  int64_t temp = 0;

  temp_barometer->D1 = Read_Barometer_Data(CMD_ADC_D1 + CMD_ADC_1024);  // Uncalibrated pressure
  temp_barometer->D2 = Read_Barometer_Data(CMD_ADC_D2 + CMD_ADC_1024);  // Uncalibrated temperature

  temp_barometer->dT = temp_barometer->D2
      - (temp_barometer->calibrationCoefficient[5] << 8);  //     dT = D2 - C5 * 2^8
  temp_barometer->baro_temp = temp_barometer->dT
      * temp_barometer->calibrationCoefficient[6];       // |
  temp_barometer->baro_temp = temp_barometer->baro_temp >> 23;              // |
  temp_barometer->temperature = temp_barometer->baro_temp + 2000;  // +-> 2000 + dT * C6 / 2^23
  temp_barometer->temperature = temp_barometer->temperature / 100.0;

  temp_barometer->baro_temp = (temp_barometer->calibrationCoefficient[2] << 10);  // | ???
  temp_barometer->baro_temp = (temp_barometer->baro_temp << 7);         // | ???
  temp_barometer->offset = ((temp_barometer->calibrationCoefficient[4]
      * temp_barometer->dT) >> 6);  // |
  temp_barometer->offset = temp_barometer->offset + temp_barometer->baro_temp;  // +-> C2 * 2^17 + (C4 * dT) / 2^6

  temp_barometer->baro_temp = temp_barometer->calibrationCoefficient[1] << 16;  // |
  temp_barometer->sensitivity = (temp_barometer->calibrationCoefficient[3]
      * temp_barometer->dT) >> 7;  // |
  temp_barometer->sensitivity = temp_barometer->sensitivity
      + temp_barometer->baro_temp;            // +-> C1 * 2^16 + (C3 * dT) / 2^7

  temp = temp_barometer->D1 * temp_barometer->sensitivity;
  temp = temp >> 11;
  temp = temp >> 10;
  temp = temp - temp_barometer->offset;
  temp = temp >> 15;

  temp_barometer->pressure = temp / 100.0;  // +-> (D1 * SENS / 2^21 - OFF) / 2^15

}

/*********************************************************************************************
 * Reset handle parameters
 * Read calibration coefficients
 * Set init argument inside handle
 *
 * ARGUMENTS :
 * 		- Barometer_t
 *
 * RETURN :
 * 		- void
 *********************************************************************************************/
void Init_Barometer(Barometer_t * temp_barometer) {
  uint32_t i = 0;

  //initialise the barometer parameters
  temp_barometer->isInitialized = 0;

  temp_barometer->D1 = 0;
  temp_barometer->D2 = 0;
  temp_barometer->baro_temp = 0;
  temp_barometer->calibrationCoefficient[0] = 0;
  temp_barometer->calibrationCoefficient[1] = 0;
  temp_barometer->calibrationCoefficient[2] = 0;
  temp_barometer->calibrationCoefficient[3] = 0;
  temp_barometer->calibrationCoefficient[4] = 0;
  temp_barometer->calibrationCoefficient[5] = 0;
  temp_barometer->calibrationCoefficient[6] = 0;
  temp_barometer->calibrationCoefficient[7] = 0;
  temp_barometer->dT = 0;
  temp_barometer->offset = 0;
  temp_barometer->pressure = 0;
  temp_barometer->sensitivity = 0;
  temp_barometer->temperature = 0;

  // Reset the barometer
  Reset_Barometer();

  // Read the calibration coefficient
  for (i = 0; i < 8; i++) {
    temp_barometer->calibrationCoefficient[i] = Read_Calibration_Coefficient(i);
  }

  //set the handle to initialised
  temp_barometer->isInitialized = 1;
}

/*********************************************************************************************
 * computes the altitude from the pressure
 * given by the barometer
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		- void
 *********************************************************************************************/
float Convert_mBar_To_Altitude(float pressure) {
  return (pow(pressure / 1013.25, 1 / 5.255) - 1) * -44330.769230769;
}

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
void Compute_Air_Density(Barometer_t * temp_barometer) {
  temp_barometer->Air_Density = 100*(temp_barometer->pressure * AIR_MOLAR_MASS)
      / (GAZ_CONSTANT * (273.15 + temp_barometer->temperature));
}
