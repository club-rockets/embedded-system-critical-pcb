/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define SENSE_DROGUE_Pin GPIO_PIN_2
#define SENSE_DROGUE_GPIO_Port GPIOE
#define FIRE_DROGUE_Pin GPIO_PIN_3
#define FIRE_DROGUE_GPIO_Port GPIOE
#define FIRE_MAIN_Pin GPIO_PIN_4
#define FIRE_MAIN_GPIO_Port GPIOE
#define SENSE_MAIN_Pin GPIO_PIN_5
#define SENSE_MAIN_GPIO_Port GPIOE
#define NOT_USED_PE6_Pin GPIO_PIN_6
#define NOT_USED_PE6_GPIO_Port GPIOE
#define NOT_USED_PC13_Pin GPIO_PIN_13
#define NOT_USED_PC13_GPIO_Port GPIOC
#define NOT_USED_PC0_Pin GPIO_PIN_0
#define NOT_USED_PC0_GPIO_Port GPIOC
#define SPI2_CS_Pin GPIO_PIN_1
#define SPI2_CS_GPIO_Port GPIOC
#define NOT_USED_PA0_Pin GPIO_PIN_0
#define NOT_USED_PA0_GPIO_Port GPIOA
#define NOT_USED_PA1_Pin GPIO_PIN_1
#define NOT_USED_PA1_GPIO_Port GPIOA
#define NOT_USED_PA4_Pin GPIO_PIN_4
#define NOT_USED_PA4_GPIO_Port GPIOA
#define FLASH2_CS1_Pin GPIO_PIN_4
#define FLASH2_CS1_GPIO_Port GPIOC
#define FLASH2_CS2_Pin GPIO_PIN_5
#define FLASH2_CS2_GPIO_Port GPIOC
#define FLASH2_WP_Pin GPIO_PIN_0
#define FLASH2_WP_GPIO_Port GPIOB
#define FLASH_HOLD_Pin GPIO_PIN_1
#define FLASH_HOLD_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_2
#define BUZZER_GPIO_Port GPIOB
#define FLASH1_CS1_Pin GPIO_PIN_7
#define FLASH1_CS1_GPIO_Port GPIOE
#define FLASH1_CS2_Pin GPIO_PIN_8
#define FLASH1_CS2_GPIO_Port GPIOE
#define SENSE_VUSB_Pin GPIO_PIN_9
#define SENSE_VUSB_GPIO_Port GPIOE
#define FLASH2_RESET_Pin GPIO_PIN_10
#define FLASH2_RESET_GPIO_Port GPIOE
#define FLASH1_RESET_Pin GPIO_PIN_11
#define FLASH1_RESET_GPIO_Port GPIOE
#define NOT_USED_PE12_Pin GPIO_PIN_12
#define NOT_USED_PE12_GPIO_Port GPIOE
#define NOT_USED_PE13_Pin GPIO_PIN_13
#define NOT_USED_PE13_GPIO_Port GPIOE
#define SENSE_EVENT_A_Pin GPIO_PIN_14
#define SENSE_EVENT_A_GPIO_Port GPIOE
#define FIRE_EVENT_A_Pin GPIO_PIN_15
#define FIRE_EVENT_A_GPIO_Port GPIOE
#define NOT_USED_PB11_Pin GPIO_PIN_11
#define NOT_USED_PB11_GPIO_Port GPIOB
#define CAN_TRANS_SLEEP_Pin GPIO_PIN_14
#define CAN_TRANS_SLEEP_GPIO_Port GPIOB
#define SENSE_VBAT_Pin GPIO_PIN_15
#define SENSE_VBAT_GPIO_Port GPIOB
#define LED_PROC_GOOD_Pin GPIO_PIN_8
#define LED_PROC_GOOD_GPIO_Port GPIOD
#define LED_GPS_SYNC_Pin GPIO_PIN_9
#define LED_GPS_SYNC_GPIO_Port GPIOD
#define LED_MAIN_ARMED_Pin GPIO_PIN_10
#define LED_MAIN_ARMED_GPIO_Port GPIOD
#define LED_DROGUE_ARMED_Pin GPIO_PIN_11
#define LED_DROGUE_ARMED_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_15
#define LED4_GPIO_Port GPIOD
#define NOT_USED_PC6_Pin GPIO_PIN_6
#define NOT_USED_PC6_GPIO_Port GPIOC
#define NOT_USED_PC7_Pin GPIO_PIN_7
#define NOT_USED_PC7_GPIO_Port GPIOC
#define NOT_USED_PA15_Pin GPIO_PIN_15
#define NOT_USED_PA15_GPIO_Port GPIOA
#define NOT_USED_PD0_Pin GPIO_PIN_0
#define NOT_USED_PD0_GPIO_Port GPIOD
#define NOT_USED_PD1_Pin GPIO_PIN_1
#define NOT_USED_PD1_GPIO_Port GPIOD
#define MicroSDCard_Detect_Pin GPIO_PIN_3
#define MicroSDCard_Detect_GPIO_Port GPIOD
#define MicroSDCard_Detect_EXTI_IRQn EXTI3_IRQn
#define NOT_USED_PD4_Pin GPIO_PIN_4
#define NOT_USED_PD4_GPIO_Port GPIOD
#define SENSE_EVENT_B_Pin GPIO_PIN_5
#define SENSE_EVENT_B_GPIO_Port GPIOD
#define FIRE_EVENT_B_Pin GPIO_PIN_6
#define FIRE_EVENT_B_GPIO_Port GPIOD
#define NOT_USED_PD7_Pin GPIO_PIN_7
#define NOT_USED_PD7_GPIO_Port GPIOD
#define NOT_USED_PB4_Pin GPIO_PIN_4
#define NOT_USED_PB4_GPIO_Port GPIOB
#define NOT_USED_PB5_Pin GPIO_PIN_5
#define NOT_USED_PB5_GPIO_Port GPIOB
#define NOT_USED_PB6_Pin GPIO_PIN_6
#define NOT_USED_PB6_GPIO_Port GPIOB
#define NOT_USED_PB7_Pin GPIO_PIN_7
#define NOT_USED_PB7_GPIO_Port GPIOB
#define NOT_USED_PB8_Pin GPIO_PIN_8
#define NOT_USED_PB8_GPIO_Port GPIOB
#define NOT_USED_PB9_Pin GPIO_PIN_9
#define NOT_USED_PB9_GPIO_Port GPIOB
#define NOT_USED_PE0_Pin GPIO_PIN_0
#define NOT_USED_PE0_GPIO_Port GPIOE
#define NOT_USED_PE1_Pin GPIO_PIN_1
#define NOT_USED_PE1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
