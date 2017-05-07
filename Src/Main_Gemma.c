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

 this file contain the main program
 every ISR is called here and passes global variable





 *********************************************************************************************/
#include "Main_Gemma.h"

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
void Init_rocket(Rockets_t * temp_rocket) {
  temp_rocket->Rocket_State = INITIALISATION;
  temp_rocket->Main_Loop_Time_Step = 50;
  temp_rocket->Main_Compute_Time = 0;
  temp_rocket->Mission_Time = 0;

  temp_rocket->Main_Parachute = &Main_Parachute;
  temp_rocket->Drogue_Parachute = &Drogue_Parachute;
  temp_rocket->Buzzer = &Buzzer;
  temp_rocket->Telemetry = &Telemetry;
  temp_rocket->Barometer = &Barometer;
  temp_rocket->Altimeter = &Altimeter;
  temp_rocket->LED = &LED;
  temp_rocket->Kalman_Altitude = &Kalman_Altitude;
  temp_rocket->Inertial_Station = &Inertial_Station;
  temp_rocket->RFD900 = &RFD900;

  temp_rocket->isInitialized = 1;
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
void separateDecimalValue(float_t value, int16_t * buff) {
  float_t temp1 = 0, temp2 = 0;
  temp1 = modff(value, &temp2);

  if (value < 0) {
    buff[0] = -1;
    buff[1] = (int16_t) (temp2 * -1);
    buff[2] = (int16_t) (temp1 * 1000 * -1);  //nombre de zero = nombre de decimale
  } else {
    buff[0] = 1;
    buff[1] = (int16_t) temp2;
    buff[2] = (int16_t) (temp1 * 1000);
  }
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
void Get_State_String(Rockets_t * temp_rocket, uint8_t * buff) {

  switch (temp_rocket->Rocket_State) {
    case 0:
      strcpy((char*) buff, "INITIALISATION");
      break;
    case 1:
      strcpy((char*) buff, "STANDBY_ON_PAD");
      break;
    case 2:
      strcpy((char*) buff, "LAUNCH");
      break;
    case 3:
      strcpy((char*) buff, "POWERED_ASCENT");
      break;
    case 4:
      strcpy((char*) buff, "ENGINE_BURNOUT");
      break;
    case 5:
      strcpy((char*) buff, "COASTING_ASCENT");
      break;
    case 6:
      strcpy((char*) buff, "APOGEE_REACHED");
      break;
    case 7:
      strcpy((char*) buff, "DROGUE_DEPLOYMENT");
      break;
    case 8:
      strcpy((char*) buff, "DROGUE_DESCEND");
      break;
    case 9:
      strcpy((char*) buff, "MAIN_DEPLOYMENT");
      break;
    case 10:
      strcpy((char*) buff, "MAIN_DESCEND");
      break;
    case 11:
      strcpy((char*) buff, "LANDING");
      break;
    case 12:
      strcpy((char*) buff, "RECOVERY");
      break;
    case 13:
      strcpy((char*) buff, "PICKEDUP");
      break;
    default:
      // Should never get here
      strcpy((char*) buff, "ERROR - INVALID ROCKET STATE");
      break;
  }

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
void State_Manager(Rockets_t * temp_rocket) {

  switch (temp_rocket->Rocket_State) {

    case INITIALISATION:

      temp_rocket->Rocket_State = STANDBY_ON_PAD;

      break;

    case STANDBY_ON_PAD:
      //on detecte le lancement avec l'acceleration
      // safety: si l'altitude est assez grande on skip...
      if (temp_rocket->Altimeter->Filtered_Acceleration
          > temp_rocket->Altimeter->Launch_Acceleration_trigger
          || (temp_rocket->Altimeter->AGL_Altitude
              > temp_rocket->Altimeter->Flight_Altitude_trigger)) {
        temp_rocket->Rocket_State = LAUNCH;
      } else {
        temp_rocket->Rocket_State = STANDBY_ON_PAD;
      }

      break;

    case LAUNCH:
      if (temp_rocket->Altimeter->AGL_Altitude
          > temp_rocket->Altimeter->Flight_Altitude_trigger) {
        temp_rocket->Rocket_State = POWERED_ASCENT;
      } else {
        temp_rocket->Rocket_State = LAUNCH;
      }
      break;

    case POWERED_ASCENT:
      if (temp_rocket->Altimeter->Burnout_Detected) {
        temp_rocket->Rocket_State = ENGINE_BURNOUT;
      } else {
        temp_rocket->Rocket_State = POWERED_ASCENT;
      }
      break;

    case ENGINE_BURNOUT:
      temp_rocket->Rocket_State = COASTING_ASCENT;
      break;

    case COASTING_ASCENT:
      if (temp_rocket->Altimeter->Apogee_Detected) {
        temp_rocket->Rocket_State = APOGEE_REACHED;
      } else {
        temp_rocket->Rocket_State = COASTING_ASCENT;
      }

      break;

      //l'etat d'apogee ne dure qu'un time step
    case APOGEE_REACHED:
      temp_rocket->Rocket_State = DROGUE_DEPLOYMENT;
      break;

    case DROGUE_DEPLOYMENT:
      //envoie un nombre defini de pulse, puis passe a l'etat suivant
      if (temp_rocket->Drogue_Parachute->Ejection_Toggle_Number == 0) {
        temp_rocket->Rocket_State = DROGUE_DESCENT;
      } else {
        temp_rocket->Rocket_State = DROGUE_DEPLOYMENT;
      }

      break;

    case DROGUE_DESCENT:
      if (Altimeter.AGL_Altitude < Altimeter.Main_Ejection_Altitude) {
        RocketsVar.Rocket_State = MAIN_DEPLOYMENT;
      } else {
        RocketsVar.Rocket_State = DROGUE_DESCENT;
      }

      break;

    case MAIN_DEPLOYMENT:
      //envoie un nombre defini de pulse, puis passe a l'etat suivant
      if (temp_rocket->Main_Parachute->Ejection_Toggle_Number == 0) {
        temp_rocket->Rocket_State = MAIN_DESCENT;
      } else {
        temp_rocket->Rocket_State = MAIN_DEPLOYMENT;
      }
      break;

    case MAIN_DESCENT:
      //lorsque la fusee atteint le meme threshold que pour le flight mode
      if (temp_rocket->Altimeter->AGL_Altitude
          < temp_rocket->Altimeter->Flight_Altitude_trigger) {
        temp_rocket->Rocket_State = LANDING;
      } else {
        temp_rocket->Rocket_State = MAIN_DESCENT;
      }
      break;

    case LANDING:
      //lorsque la vitesse augmente, l'acceleration passe de neg. a pos.
      if (temp_rocket->Altimeter->Filtered_Acceleration > 0) {
        temp_rocket->Rocket_State = RECOVERY;
      }
      break;

    case RECOVERY:
      //si la position varie, la vitesse va varier...
      if (fabs(temp_rocket->Altimeter->Filtered_Velocity) > 2) {
        temp_rocket->Rocket_State = PICKEDUP;
      }
      break;

    case PICKEDUP:
      temp_rocket->Rocket_State = PICKEDUP;
      break;

    default:
      break;
  }

}

/*********************************************************************************************
 *	TIMER 1-2-3 callback
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim) {
  static uint32_t loop_counter, save_counter;
  /*********************************************************************************************
   *	Main program interrupt loop where the state machine of the rocket is implemented
   *	called by timer handler opon ellapsed period
   * ARGUMENTS :
   * 		-
   *
   * RETURN :
   * 		-
   *
   *********************************************************************************************/
  if (htim->Instance == TIM1) {

    LED.Critical_LED1 ^= 1;  //toggle each loop input

    switch (RocketsVar.Rocket_State) {

      /*******************************************************************
       * initialisation des modules
       * le tim1 est arreter pour laisser le temps...
       *
       *
       *******************************************************************/
      case INITIALISATION:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        HAL_TIM_Base_Stop_IT(htim);
        LED.Critical_LED4 = 0;

        /***************************************************
         * init begin
         ***************************************************/

        Init_rocket(&RocketsVar);
        Init_LED(&LED);
        Init_Buzzer(&Buzzer);
        Init_Altimeter(&Altimeter);

        Init_Barometer(&Barometer);
        Init_Telemetry(&Telemetry);
        Init_RFD900(&RFD900);
        Init_Inertial_Station(&Inertial_Station);
        Init_Kalman(&Kalman_Altitude);
        Init_LP_Filter(&LP_Filter);

        //init non volatile memory (BACKUP SRAM)
        //read values of settings before power cycle in init fct
        //stored in backup sram
        Init_Backup_SRAM(&Backup_Settings);
        //overwrite settings with back up data as init value
        Altimeter.Apogee_Ejection_Delay = Get_Backup_Apogee_Delay(
            &Backup_Settings);
        Altimeter.Ultrasonic_Delay = Get_Backup_Sonic_Delay(&Backup_Settings);
        Altimeter.Main_Ejection_Altitude = Get_Backup_Altitude_Main(
            &Backup_Settings);

        //init de fatfs et de la SD
        if (BSP_SD_Init() == 0) {
          f_mount(&fs, (TCHAR const*) SD_Path, 1);
          f_open(&data_file, FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
          f_lseek(&data_file, f_size(&data_file));

          //Log file header
          strcpy((char*) Save_String, DATA_LOG_HEADER);
          f_puts((char*) Save_String, &data_file);

          //column title
          strcpy((char*) Save_String, DATA_LOG_COL_NAME);
          f_puts((char*) Save_String, &data_file);

          //write file to sd
          f_close(&data_file);
        }

        HAL_TIM_Base_Start_IT(htim);
        break;

        /*******************************************************************
         * waiting on the pad for launch
         *
         *
         *
         *******************************************************************/
      case STANDBY_ON_PAD:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        LED.Critical_LED4 = 1;
        LED.Critical_LED3 = 1;
        Buzzer.Buzzer_enable = 1;

        /***************************************************
         * update du buzzer
         ***************************************************/
        if (Drogue_Parachute.Ejection_Charge_Detect
            && Main_Parachute.Ejection_Charge_Detect) {
          Buzzer.Buzzer_mode = 3;
        } else if (Drogue_Parachute.Ejection_Charge_Detect
            && !Main_Parachute.Ejection_Charge_Detect) {
          Buzzer.Buzzer_mode = 2;
        } else if (!Drogue_Parachute.Ejection_Charge_Detect
            && Main_Parachute.Ejection_Charge_Detect) {
          Buzzer.Buzzer_mode = 1;
        } else {
          Buzzer.Buzzer_mode = 0;
        }

        break;

        /*******************************************************************
         * exiting launch pad
         *
         *
         *
         *******************************************************************/
      case LAUNCH:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        LED.Critical_LED4 = 0;
        LED.Critical_LED3 = 0;
        Buzzer.Buzzer_enable = 0;
        Telemetry.Loop_Step = 1; //each 4x main loop

        break;

        /*******************************************************************
         * engine is running
         *
         *
         *
         *******************************************************************/
      case POWERED_ASCENT:

        /***************************************************
         * detection du burnout apres delay ultrasonic
         * enlever le IF s'il y a un accelerometre dans
         * le filtre de kalman
         ***************************************************/
        if (RocketsVar.Mission_Time > Altimeter.Ultrasonic_Delay) {
          Burnout_Detection(&Altimeter);
        }
        break;

        /*******************************************************************
         * engine is stoping
         *
         *
         *
         *******************************************************************/
      case ENGINE_BURNOUT:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        LED.Critical_LED4 = 1;
        LED.Critical_LED3 = 0;
        Altimeter.Burnout_Time = RocketsVar.Mission_Time;
        break;

        /*******************************************************************
         * rocket is gliding towards sky
         *
         *
         *
         *******************************************************************/
      case COASTING_ASCENT:

        /***************************************************
         *	detection d'apogee apres delais supersonic
         *  avec un accelerometre, tester pour enlever dly
         ***************************************************/
        if (RocketsVar.Mission_Time > Altimeter.Ultrasonic_Delay) {
          Apogee_Detection(&Altimeter);
        }

        break;

        /*******************************************************************
         * apogee is reached
         *
         *
         *
         *******************************************************************/
      case APOGEE_REACHED:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        LED.Critical_LED4 = 0;
        LED.Critical_LED3 = 1;

        /***************************************************
         * Enregistrement du temps à l'apogee
         ***************************************************/
        Altimeter.Apogee_Time = RocketsVar.Mission_Time;
        break;

        /*******************************************************************
         * drogue parachute is deploying
         *
         *
         *
         *******************************************************************/
      case DROGUE_DEPLOYMENT:

        /***************************************************
         * delai d'ejection du drogue apres l'apogee
         * enregistrement de l'altitude de deploiement
         ***************************************************/
        if (RocketsVar.Mission_Time
            > Altimeter.Apogee_Time + Altimeter.Apogee_Ejection_Delay) {
          Drogue_Parachute.Ejection_Charge_Fire = 1;
          Drogue_Parachute.Estimated_Altitude_Deployed = Altimeter
              .Filtered_Altitude;
          Drogue_Parachute.Barometric_Altitude_Deployed =
              Altimeter.AGL_Altitude;
        }
        break;

        /*******************************************************************
         * rocket is rapidly descending towards ground
         *
         *
         *
         *******************************************************************/
      case DROGUE_DESCENT:
        break;

        /*******************************************************************
         * main parachute is deploying
         *
         *
         *
         *******************************************************************/
      case MAIN_DEPLOYMENT:

        /***************************************************
         * read/write des modification hardware
         ***************************************************/
        LED.Critical_LED4 = 1;
        LED.Critical_LED3 = 1;

        /***************************************************
         * ejection du main parachute
         * enregistrement de l'altitude de deploiement
         ***************************************************/
        Main_Parachute.Ejection_Charge_Fire = 1;
        Main_Parachute.Estimated_Altitude_Deployed =
            Altimeter.Filtered_Altitude;
        Main_Parachute.Barometric_Altitude_Deployed = Altimeter.AGL_Altitude;
        break;

        /*******************************************************************
         * rocket is slowly descending towards ground
         *
         *
         *
         *******************************************************************/
      case MAIN_DESCENT:
        break;

        /*******************************************************************
         * rocket is landing
         * touch down imminent
         *
         *
         *******************************************************************/
      case LANDING:
        break;

        /*******************************************************************
         * rocket is in recovery mode
         * low power mode...
         *
         *
         *******************************************************************/
      case RECOVERY:
        Telemetry.Loop_Step = 100;

        break;

        /*******************************************************************
         * rocket have been picked up
         *
         *
         *
         *******************************************************************/
      case PICKEDUP:
        break;

        /*******************************************************************
         * invalid state
         *
         *
         *
         *******************************************************************/
      default:
        break;

    }        //rocket state behavior end

    /*******************************************************************
     * from here to the end of tim1 callback function
     * is the program that is done every loop
     *
     *
     *******************************************************************/

    /***************************************************
     * update mission time, begin with launch
     ***************************************************/
    if (RocketsVar.Rocket_State >= LAUNCH
        && RocketsVar.Rocket_State <= LANDING) {
      RocketsVar.Mission_Time += RocketsVar.Main_Loop_Time_Step;
    }

    /***************************************************
     * update de la pression/temperature du barometre
     * et calcul de la densité de l'air
     ***************************************************/
    Compute_Barometer_Pressure(&Barometer);
    Compute_Air_Density(&Barometer);

    //simulator
    //overwrite pressure data only
    /*
     if (loop_counter < sizeof(PRESSURE_POINTS_HYPERION) >> 2) {
     Barometer.pressure = PRESSURE_POINTS_HYPERION[loop_counter
     % sizeof(PRESSURE_POINTS_HYPERION)];
     }
     *//*
     if (loop_counter < sizeof(PRESSURE_POINTS_HYPERION_SONIC)) {
     Barometer.pressure = PRESSURE_POINTS_HYPERION_SONIC[loop_counter
     % sizeof(PRESSURE_POINTS_HYPERION) >> 2];
     }

     if(loop_counter < sizeof(PRESSURE_POINT_AMAROK)>>2){
     Barometer.pressure = PRESSURE_POINT_AMAROK[loop_counter%sizeof(PRESSURE_POINTS_HYPERION)];
     }
     */

    /***************************************************
     * update de l'altimetre et de kalman
     ***************************************************/
    Altimeter.Barometric_Altitude = Convert_mBar_To_Altitude(
        Barometer.pressure);

    //grab sea altitude level to offset AGL altitude
    //one time
    if (RocketsVar.Rocket_State == INITIALISATION) {
      while( Altimeter.Initial_Altitude_Count){
        Compute_Barometer_Pressure(&Barometer);
        Compute_Air_Density(&Barometer);
        Compute_Initial_Altitude(&Altimeter);
        Altimeter.Initial_Altitude_Count--;
      }
    }

    Compute_AGL_Altitude(&Altimeter);

    //update kalman filter altitude data
    Kalman_Altitude.Barometric_Altitude = Altimeter.AGL_Altitude;
    Update_Kalman(&Kalman_Altitude);

    //update altimeter filtered data
    Altimeter.Filtered_Altitude = Kalman_Altitude.Filtered_Altitude;
    Altimeter.Filtered_Acceleration = Kalman_Altitude.Filtered_Acceleration;
    Altimeter.Filtered_Velocity = Kalman_Altitude.Filtered_Velocity;

    /***************************************************
     * update des user interface et e-match
     ***************************************************/
    Update_LED(&LED);
    Update_Alarm_Rocket(&Buzzer);

    Update_Drogue_Continuity(&Drogue_Parachute);
    Update_Drogue_Fire(&Drogue_Parachute);

    Update_Main_Continuity(&Main_Parachute);
    Update_Main_Fire(&Main_Parachute);

    /***************************************************
     * update real time clock variable before save
     ***************************************************/
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    /***************************************************
     * translate rocket state in string with state name
     ***************************************************/
    Get_State_String(&RocketsVar, Rocket_State_String);
    /***************************************************
     * SD save in buffer
     ***************************************************/
    sprintf(
        (char*) (Save_String),
        "20%02d-%02d-%02dT%02d:%02d:%02d,%s,%lu,%lu,%lu,%lu,%lu,%lu,%f,%f,%f,%f,%f,%f,%f\n",
        sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes,
        sTime.Seconds, Rocket_State_String, RocketsVar.Mission_Time,
        ((RocketsVar.Main_Compute_Time * 100) / RocketsVar.Main_Loop_Time_Step),
        Main_Parachute.Ejection_Charge_Detect,
        Main_Parachute.Ejection_Charge_Fire,
        Drogue_Parachute.Ejection_Charge_Detect,
        Drogue_Parachute.Ejection_Charge_Fire, Barometer.temperature,
        RocketsVar.Barometer->Air_Density, Altimeter.Barometric_Altitude,
        Altimeter.AGL_Altitude, Altimeter.Filtered_Altitude,
        Altimeter.Filtered_Velocity, Altimeter.Filtered_Acceleration);

    f_puts((TCHAR*) Save_String, &data_file);

    /***************************************************
     * Telemetrie
     ***************************************************/
    if(!(loop_counter % Telemetry.Loop_Step) && !(Telemetry.Busy)){
      cJSON * root_json;
      cJSON * measures_json;
      cJSON * altimeter_json;

      HAL_UART_Receive_IT(&huart2, (uint8_t*)Telemetry.RX_JSON_string, 2);

      Get_State_String(&RocketsVar, (uint8_t*)Telemetry.Rocket_State_String);

      root_json = cJSON_CreateObject();
      measures_json = cJSON_CreateObject();
      altimeter_json = cJSON_CreateObject();

      cJSON_AddItemToObject(root_json, "ID",
                            cJSON_CreateString(Telemetry.Telemetry_ID));
      cJSON_AddItemToObject(root_json, "State",
                            cJSON_CreateString(Telemetry.Rocket_State_String));

      cJSON_AddItemToObject(root_json, "Sensors", measures_json);
      cJSON_AddItemToObject(measures_json, "Altimeter", altimeter_json);

      cJSON_AddNumberToObject(altimeter_json, "Altitude_AGL",
                              Altimeter.AGL_Altitude);

      cJSON_AddNumberToObject(altimeter_json, "Estimated_Altitude",
                              Altimeter.Filtered_Altitude);

      cJSON_AddNumberToObject(altimeter_json, "Estimated_Velocity",
                              Altimeter.Filtered_Velocity);

      cJSON_AddNumberToObject(altimeter_json, "Estimated_Acceleration",
                              Altimeter.Filtered_Acceleration);

      Telemetry.TX_JSON_string = cJSON_PrintUnformatted((root_json));

      //realloc JSON string with another case to add "\n"
      Telemetry.TX_JSON_Base_Station = malloc(strlen(Telemetry.TX_JSON_string) + 10);
      strcpy(Telemetry.TX_JSON_Base_Station,Telemetry.TX_JSON_string);
      free(Telemetry.TX_JSON_string);
      strcat(Telemetry.TX_JSON_Base_Station, "\n");
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)Telemetry.TX_JSON_Base_Station, strlen(Telemetry.TX_JSON_Base_Station));
      Telemetry.Busy = 1;

      //free(Telemetry.TX_JSON_string);
      cJSON_Delete(root_json);
    }
    /***************************************************
     * update rocket state
     ***************************************************/
    State_Manager(&RocketsVar);
    loop_counter++;

    /***************************************************
     * USB SERIAL COM PORT - programation de l'altimetre
     ***************************************************/

    //code test pour programmer l'horloge
    //est presentement utilisé avec des script sur teraterm
    //rx is done elsewhere in usb_cdc_if.c
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
      volatile uint8_t val = 0;
      if (USB_CDC_RX[0] == 'd' && USB_CDC_RX[1] == 'a' && USB_CDC_RX[2] == 't'
          && USB_CDC_RX[3] == 'e') {
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        sDate.Date = atoi(&USB_CDC_RX[4]);
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        itoa(sDate.Date, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
        USB_CDC_RX[3] = 0;
        USB_CDC_RX[4] = 0;
        USB_CDC_RX[5] = 0;
        USB_CDC_RX[6] = 0;
      }

      if (USB_CDC_RX[0] == 'h' && USB_CDC_RX[1] == 'o' && USB_CDC_RX[2] == 'u'
          && USB_CDC_RX[3] == 'r') {
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        sTime.Hours = atoi(&USB_CDC_RX[4]);
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        itoa(sTime.Hours, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
        USB_CDC_RX[3] = 0;
        USB_CDC_RX[4] = 0;
        USB_CDC_RX[5] = 0;
        USB_CDC_RX[6] = 0;
      }

      if (USB_CDC_RX[0] == 'y' && USB_CDC_RX[1] == 'e' && USB_CDC_RX[2] == 'a'
          && USB_CDC_RX[3] == 'r') {
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        sDate.Year = atoi(&USB_CDC_RX[4]);
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        itoa(sDate.Year, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
        USB_CDC_RX[3] = 0;
        USB_CDC_RX[4] = 0;
        USB_CDC_RX[5] = 0;
        USB_CDC_RX[6] = 0;
      }

      if (USB_CDC_RX[0] == 'm' && USB_CDC_RX[1] == 'o' && USB_CDC_RX[2] == 'n'
          && USB_CDC_RX[3] == 't' && USB_CDC_RX[4] == 'h') {
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        sDate.Month = atoi(&USB_CDC_RX[5]);
        HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        itoa(sDate.Month, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
        USB_CDC_RX[3] = 0;
        USB_CDC_RX[4] = 0;
        USB_CDC_RX[5] = 0;
        USB_CDC_RX[6] = 0;
      }

      if (USB_CDC_RX[0] == 'm' && USB_CDC_RX[1] == 'i'
          && USB_CDC_RX[2] == 'n') {
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        sTime.Minutes = atoi(&USB_CDC_RX[3]);
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        itoa(sTime.Minutes, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
        USB_CDC_RX[3] = 0;
        USB_CDC_RX[4] = 0;
        USB_CDC_RX[5] = 0;
        USB_CDC_RX[6] = 0;
      }

      if (USB_CDC_RX[0] == 's' && USB_CDC_RX[1] == 'e'
          && USB_CDC_RX[2] == 'c') {
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        sTime.Seconds = atoi(&USB_CDC_RX[3]);
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        itoa(sTime.Seconds, USB_CDC_TX, 10);
        CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
        USB_CDC_RX[0] = 0;
      }
    }

    /***************************************************
     * Compute main loop time tick-toc
     ***************************************************/
    RocketsVar.Main_Compute_Time =
        (uint32_t) ((((float_t) __HAL_TIM_GET_COUNTER(htim)) * 0.00152592547));

  }    //TIm1 callback function end

  /*********************************************************************************************
   *	TIM2 callback
   *	sd save on card and settings on backup SRAM program interrupt loop
   *	called by timer handler opon number of mainloop count, clock is tim1 output
   *
   *	write each 100 cycle = 100 * 50ms = 5seconde
   *	in sd card and back sram
   *
   *	try reset after debug if program jam...
   *
   *
   * ARGUMENTS :
   * 		-
   *
   * RETURN :
   * 		-
   *
   *********************************************************************************************/
  if (htim->Instance == TIM2) {

    //turn on led is the data as been saved
    if (f_close(&data_file) == FR_OK) {
      LED.Critical_LED2 = 1;
    }
    //if error while writing, reset led
    else {
      LED.Critical_LED2 = 0;
    }

    f_open(&data_file, FILENAME, FA_OPEN_EXISTING | FA_WRITE);
    f_lseek(&data_file, f_size(&data_file));

    //save user setting to Backup SRAM
    //this ram sector wont erase upon power cycling
    Save_Backup_SRAM(&Backup_Settings);

    save_counter++;
  }

  /*********************************************************************************************
   *	TIM3 callback
   *	called by timer handler opon number of mainloop count, clock is tim1 output
   *	value is 20 x 50ms = 1seconde
   *	time can change upon discovery mode
   *
   * ARGUMENTS :
   * 		-
   *
   * RETURN :
   * 		-
   *
   *********************************************************************************************/
  if (htim->Instance == TIM3) {

  }

}




/*********************************************************************************************
 *	CAN2 RX callback
 *
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
  if (hcan->Instance == CAN2) {

    switch (hcan->pRxMsg->StdId) {

      case CAN_GPS_LONGITUDE_ID:
        memcpy(&Inertial_Station.GPS_longitude, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.GPS_longitude));
        break;
      case CAN_GPS_LATITUDE_ID:
        memcpy(&Inertial_Station.GPS_latitude, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.GPS_latitude));
        break;
      case CAN_GPS_ALTITUDE_ID:
        memcpy(&Inertial_Station.GPS_altitude, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.GPS_altitude));
        break;
      case CAN_GPS_FIX_TYPE_ID:
        memcpy(&Inertial_Station.Fix_Type, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.Fix_Type));
        break;
      case CAN_GPS_LOCKED_ID:
        memcpy(&Inertial_Station.GPS_locked, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.GPS_locked));
        break;
      case CAN_GPS_N_SATELLITE_ID:
        memcpy(&Inertial_Station.GPS_N_satellite, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.GPS_N_satellite));
        break;
      case CAN_ACCELERATION_X_ID:
        memcpy(&Inertial_Station.x_acceleration, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.x_acceleration));
        break;
      case CAN_ACCELERATION_Y_ID:
        memcpy(&Inertial_Station.y_acceleration, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.y_acceleration));
        break;
      case CAN_ACCELERATION_Z_ID:
        memcpy(&Inertial_Station.z_acceleration, hcan->pRxMsg->Data,
               sizeof(Inertial_Station.z_acceleration));
        break;
      default:
        // do nothing
        break;
    }
  }
}

/*********************************************************************************************
 *	UART2 RX callback
 *	RFD900
 *	when uart_receive_IT(size) is started and @size@ number of char is received
 *	this ISR is called
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  if (huart->Instance == USART2) {

    if (RFD900.Wait_for_OK_IT == 1) {

      if ((RFD900.RX[0] = 0x4F) && (RFD900.RX[1] == 0x4B)) {  // if = 'O' 'K'
        RFD900.Wait_for_OK_IT = 0;
      }
      RFD900.RX[0] = 0;
      RFD900.RX[1] = 0;
    } else {
      //decode JSON input string


    }

  }
}

/*********************************************************************************************
 * interrupt for end of DMA xfer on usart
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  if (huart->Instance == USART2) {
    free(Telemetry.TX_JSON_Base_Station);
    Telemetry.Busy = 0;
  }
}

/*********************************************************************************************
 *	SD DETECT PIN callback
 *	done opon a rising or falling edge of the sd card
 *	bounce on pin is poor, so it may be called multiple times upon event
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  static int i;
  i++;
  //recall all init process
  //pretty slow, but works quite well
  if (BSP_SD_Init() == 0) {
    f_mount(&fs, (TCHAR const*) SD_Path, 1);
    f_open(&data_file, FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
    f_lseek(&data_file, f_size(&data_file));

    //Log file header
    strcpy((char*) Save_String, DATA_LOG_HEADER);
    f_puts((char*) Save_String, &data_file);

    //column title
    strcpy((char*) Save_String, DATA_LOG_COL_NAME);
    f_puts((char*) Save_String, &data_file);

    //write file to sd
    f_close(&data_file);
  }
}

