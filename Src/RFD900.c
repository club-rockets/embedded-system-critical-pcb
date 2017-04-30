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
#include "RFD900.h"

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
void Init_RFD900(RFD900_t * RF_modem) {
  RF_modem->isInitialized = 0;
  RF_modem->Wait_for_OK_IT = 0;

  RF_modem->Format = 0;
  RF_modem->Serial_Speed = 0;
  RF_modem->Air_Speed = 0;
  RF_modem->Net_ID = 0;

  //toggle init power depending on alimentation port
  //if powered up with USB, 30dB will drop the VBUS
  if (!HAL_GPIO_ReadPin(SENSE_VUSB_GPIO_Port, SENSE_VUSB_Pin)) {
    RF_modem->TX_Power = 1;
  } else if (!HAL_GPIO_ReadPin(SENSE_VBAT_GPIO_Port, SENSE_VBAT_Pin)) {
    RF_modem->TX_Power = 30;
  } else {
    RF_modem->TX_Power = 0;
  }

  RF_modem->ECC = 0;
  RF_modem->Mavlink = 0;
  RF_modem->Op_Resend = 0;
  RF_modem->Min_Frequency = 0;
  RF_modem->Max_Frequency = 0;
  RF_modem->Number_of_Channels = 0;
  RF_modem->Duty_Cycle = 0;
  RF_modem->LBT_RSSI = 0;
  RF_modem->Manchester = 0;
  RF_modem->RTSCTS = 0;
  RF_modem->Node_ID = 0;
  RF_modem->Node_Destination = 0;
  RF_modem->Sync_Any = 0;
  RF_modem->Node_Count = 0;

  RFD900_write_TX_Power(RF_modem);

  RF_modem->isInitialized = 1;
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
void RFD900_reboot(void) {
  const uint8_t reboot[5] = { 'A', 'T', 'Z', '\r', '\n' };
  const uint8_t enter[2] = { '\r', '\n' };

  //envoie la cmd ATZ
  HAL_UART_Transmit(&huart2, (uint8_t*) reboot, sizeof(reboot), 1);
  HAL_Delay(3000);
  //send <enter> for safety
  HAL_UART_Transmit(&huart2, (uint8_t*) enter, sizeof(enter), 1);
}

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 * not tested
 *********************************************************************************************/
void RFD900_reset_factory(void) {
  const uint8_t factory_default[6] = { 'A', 'T', '&', 'F', '\r', '\n' };
  const uint8_t enter[2] = { '\r', '\n' };

  //reset factory setting
  //baudrate will be 57600!
  HAL_UART_Transmit(&huart2, (uint8_t*) factory_default,
                    sizeof(factory_default), 100);
  HAL_Delay(3000);

  //reboot radio
  RFD900_reboot();

  //send <enter> for safety
  HAL_UART_Transmit(&huart2, (uint8_t*) enter, sizeof(enter), 100);
}

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 * not working, probe RFD900tool for protocole, dont trust the ref manual ......
 *********************************************************************************************/
void RFD900_read_setting(RFD900_t * RF_modem) {
  const uint8_t AT_quit[5] = { 'A', 'T', 'O', '\r', '\n' };
  const uint8_t AT_mode[3] = { '+', '+', '+' };
  const uint8_t read_power[7] = { 'A', 'T', 'S', '4', '?', '\r', '\n' };
  const uint8_t enter[2] = { '\r', '\n' };
  uint32_t loop_counter = 0;

  //send <enter> for safety
  HAL_UART_Transmit(&huart2, (uint8_t*) enter, sizeof(enter), 1);

  //enter AT mode to program the modem
  //send AT_mode cmd then wait for OK sent from modem (in ISR)
  //if no modem, timeout via loop_counter
  HAL_UART_Transmit(&huart2, (uint8_t*) enter, sizeof(enter), 1);
  HAL_Delay(1000);
  HAL_UART_Transmit(&huart2, (uint8_t*) AT_mode, sizeof(AT_mode), 1);
  HAL_UART_Receive_IT(&huart2, RF_modem->RX, 2);
  RF_modem->Wait_for_OK_IT = 1;
  while (RF_modem->Wait_for_OK_IT && loop_counter < 0x07fffffe)
    loop_counter++;

  //if timeout, dont do anything else there's no modem
  if (loop_counter < 0x07fffffe) {
    //attend 100ms apres le avoir entré dans le reg edit
    HAL_Delay(100);

    //send read tx power command
    HAL_UART_Transmit(&huart2, (uint8_t*) read_power, sizeof(read_power), 100);
    //need to probe rx to know what to look for
    //will not work written like this and nothing is done in the interrupt yet

    //quit AT mode
    HAL_UART_Transmit(&huart2, (uint8_t*) AT_quit, sizeof(AT_quit), 100);
  }
}

/*********************************************************************************************
 *	RFD900 write TX power
 *	send the tx power stored in the argument RF_modem to the physical device
 *
 * ARGUMENTS :
 * 			- RF_modem_t * RF_modem
 * 			variable used to handle RF900 attribute
 *
 * RETURN :
 * 			- void
 *
 *********************************************************************************************/
void RFD900_write_TX_Power(RFD900_t * RF_modem) {
  const uint8_t AT_mode[3] = { '+', '+', '+' };
  const uint8_t write_mem[8] = { '\r', '\n', 'A', 'T', '&', 'W', '\r', '\n' };
  const uint8_t enter[2] = { '\r', '\n' };
  const uint8_t power_cmd[13] = { 0 };

  uint16_t TX_Power_digits[2] = { 0 }, power_cmd_size = 0;
  uint32_t loop_counter = 0;

  //limite superieure
  if (RF_modem->TX_Power > 30) {
    RF_modem->TX_Power = 30;
  }
  //limite inferieure
  if (RF_modem->TX_Power < 0) {
    RF_modem->TX_Power = 0;
  }

  //convert tx power to AT command
  if (RF_modem->TX_Power >= 10) {
    TX_Power_digits[0] = RF_modem->TX_Power / 10;
    TX_Power_digits[1] = RF_modem->TX_Power - TX_Power_digits[0];
    sprintf((char*) power_cmd, "\r\nATS4=%u%u\r\n", TX_Power_digits[0],
            TX_Power_digits[1]);
    power_cmd_size = 11;
  } else {
    TX_Power_digits[0] = RF_modem->TX_Power / 10;
    sprintf((char*) power_cmd, "\r\nATS4=%u\r\n", TX_Power_digits[0]);
    power_cmd_size = 10;
  }

  //enter AT mode to program the modem
  //send AT_mode cmd then wait for OK sent from modem (in ISR)
  //if no modem, timeout via loop_counter
  HAL_UART_Transmit(&huart2, (uint8_t*) enter, sizeof(enter), 1);
  HAL_Delay(1000);
  HAL_UART_Transmit(&huart2, (uint8_t*) AT_mode, sizeof(AT_mode), 1);
  HAL_UART_Receive_IT(&huart2, RF_modem->RX, 2);
  RF_modem->Wait_for_OK_IT = 1;
  while (RF_modem->Wait_for_OK_IT && loop_counter < 0x07fffffe)
    loop_counter++;

  //if timeout, dont do anything else there's no modem
  if (loop_counter < 0x07fffffe) {
    //attend 100ms apres le avoir entré dans le reg edit
    HAL_Delay(100);

    //while in AT mode, modify the transmit power
    HAL_UART_Transmit(&huart2, (uint8_t*) power_cmd, power_cmd_size, 1);
    //HAL_UART_Receive_IT(&huart2, RF_modem->RX, 1);
    //RF_modem->Wait_for_OK_IT = 1;
    //while(RF_modem->Wait_for_OK_IT);
    HAL_Delay(100);

    //use write reg command
    //send AT_mode cmd
    HAL_UART_Transmit(&huart2, (uint8_t*) write_mem, sizeof(write_mem), 1);
    //HAL_UART_Receive_IT(&huart2, RF_modem->RX, 4);
    //RF_modem->Wait_for_OK_IT = 1;
    //while(RF_modem->Wait_for_OK_IT);
    HAL_Delay(100);
    RFD900_reboot();
  }
}

