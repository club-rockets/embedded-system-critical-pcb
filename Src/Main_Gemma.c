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

 Ce fichier contient des wrapper et les routines d'inturruption principale, utilis� � l'aide de
 callback. les interruption handler sont dans stm32f4xx_it.c et appelent les callbacks (cubeMX)

 on y retrouve deux timer synchronis� (50ms et 5s) pour gerer la boucle principale et la
 sauvegarde des donn�es (SD + backup'd RAM)

 le rfd900/uart1 utilise la DMA pour xmit. un callback sur la fin de la transmission assure
 de ne pas renvoyer de data alors que la dma n'a pas fini son transfert. le receive se fait
 aussi avec un callback et la longeur de la trame attendue doit etre connu pour se faire.

 un callback est utiliser pour le CAN-receive et le SD_detect (EXTI)


 *********************************************************************************************/
#include "Main_Gemma.h"

void callback_rocket_state_step(TIM_HandleTypeDef* htim);
void callback_step_initialisation(TIM_HandleTypeDef* htim);
void callback_step_standby();
void deploy_main();
void deploy_drogue();

/*********************************************************************************************
 * Init_rocket() initialise la variable RocketsVar d�clar� dans main_gemma.h
 * RocketsVar contient certain parametre de mission (flight state, compute time, etc)
 * et elle contient aussi les address des autre managers
 *
 * ARGUMENTS :
 * 		- Rocket_t *
 *
 * RETURN :
 * 		- void
 *
 *********************************************************************************************/
void Init_rocket(Rockets_t* temp_rocket)
{
    temp_rocket->Rocket_State = INITIALISATION;
    temp_rocket->Main_Loop_Time_Step = MAIN_LOOP_TIME;
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
 * separateDecimalValue() separe ans deux integer la partie decimal et unit� d'un float
 *
 * ARGUMENTS :
 * 		- float
 * 		- int *
 *
 * RETURN :
 * 		- void
 *
 *********************************************************************************************/
void separateDecimalValue(float_t value, int16_t* buff)
{
    float_t temp1 = 0, temp2 = 0;
    temp1 = modff(value, &temp2);

    if (value < 0) {
        buff[0] = -1;
        buff[1] = (int16_t)(temp2 * -1);
        buff[2] = (int16_t)(temp1 * 1000 * -1); //nombre de zero = nombre de decimale
    } else {
        buff[0] = 1;
        buff[1] = (int16_t)temp2;
        buff[2] = (int16_t)(temp1 * 1000);
    }
}

/*********************************************************************************************
 * Get_State_String() recoit la RocketsVar et transmet dans une string l'�tat du vol
 *
 * ARGUMENTS :
 * 		- rocket_t *
 * 		- char[]
 *
 * RETURN :
 * 		- void
 *
 *********************************************************************************************/
void Get_State_String(Rockets_t* temp_rocket, uint8_t* buff)
{

    switch (temp_rocket->Rocket_State) {
    case 0:
        strcpy((char*)buff, "INITIALISATION");
        break;
    case 1:
        strcpy((char*)buff, "STANDBY_ON_PAD");
        break;
    case 2:
        strcpy((char*)buff, "LAUNCH");
        break;
    case 3:
        strcpy((char*)buff, "POWERED_ASCENT");
        break;
    case 4:
        strcpy((char*)buff, "ENGINE_BURNOUT");
        break;
    case 5:
        strcpy((char*)buff, "COASTING_ASCENT");
        break;
    case 6:
        strcpy((char*)buff, "APOGEE_REACHED");
        break;
    case 7:
        strcpy((char*)buff, "DROGUE_DEPLOYMENT");
        break;
    case 8:
        strcpy((char*)buff, "DROGUE_DESCEND");
        break;
    case 9:
        strcpy((char*)buff, "MAIN_DEPLOYMENT");
        break;
    case 10:
        strcpy((char*)buff, "MAIN_DESCEND");
        break;
    case 11:
        strcpy((char*)buff, "LANDING");
        break;
    case 12:
        strcpy((char*)buff, "RECOVERY");
        break;
    case 13:
        strcpy((char*)buff, "PICKEDUP");
        break;

    default:
        // Should never get here
        strcpy((char*)buff, "ERROR - INVALID ROCKET STATE");
        //send_data_message(MSG_ID_ERROR_INVALID_STATE, temp_rocket->Rocket_State);
        break;
    }
}

/*********************************************************************************************
 * State_Manager() g�re le changement de l'�tat du vol
 * tous les parametres sont transmit via RocketsVar
 *
 * ARGUMENTS :
 * 		- rocket_t *
 *
 * RETURN :
 * 		- void
 *
 *********************************************************************************************/
void State_Manager(Rockets_t* temp_rocket)
{

    switch (temp_rocket->Rocket_State) {

    case INITIALISATION:
        temp_rocket->Rocket_State = STANDBY_ON_PAD;
        break;

    case STANDBY_ON_PAD:
        //on detecte le lancement avec l'acceleration
        // safety: si l'altitude est assez grande on skip...
        if (temp_rocket->Altimeter->Filtered_Acceleration > temp_rocket->Altimeter->Launch_Acceleration_trigger || (temp_rocket->Altimeter->AGL_Altitude > temp_rocket->Altimeter->Flight_Altitude_trigger)) {
            temp_rocket->Rocket_State = LAUNCH;
        } else {
            temp_rocket->Rocket_State = STANDBY_ON_PAD;
        }

        break;

    case LAUNCH:
        if (temp_rocket->Altimeter->AGL_Altitude > temp_rocket->Altimeter->Flight_Altitude_trigger) {
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
        //if main ejection altitude is detected
        //starts 2sec after drogue deployment
        if (temp_rocket->Altimeter->AGL_Altitude < temp_rocket->Altimeter->Main_Ejection_Altitude && temp_rocket->Mission_Time > temp_rocket->Altimeter->Apogee_Time + 2000 + temp_rocket->Altimeter->Apogee_Ejection_Delay) {
            temp_rocket->Rocket_State = MAIN_DEPLOYMENT;
        } else {
            temp_rocket->Rocket_State = DROGUE_DESCENT;
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
        if (temp_rocket->Altimeter->AGL_Altitude < temp_rocket->Altimeter->Flight_Altitude_trigger) {
            temp_rocket->Rocket_State = LANDING;
        } else {
            temp_rocket->Rocket_State = MAIN_DESCENT;
        }
        break;

    case LANDING:
        //lorsque la vitesse n'est plus negative,
        if (temp_rocket->Altimeter->Filtered_Velocity > 0) {
            temp_rocket->Rocket_State = RECOVERY;
        }
        break;

    case RECOVERY:
        //si la position varie, la vitesse va varier...
        //marche tres mal, mais bon ^(-_-)^
        if (fabs(temp_rocket->Altimeter->Filtered_Velocity) > PICKUP_VELOCITY_TRIGGER) {
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
 * TIMER 1-2-3 callback
 * recoit le timer handler, un if doit etre fait pour savoir quel timer a fait un interruption
 *
 * TIM1 = main loop
 * TIM2 = sauvegarde
 *
 * ARGUMENTS :
 * 		- htim *
 *
 * RETURN :
 * 		- void
 *
 *********************************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
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

        LED.Critical_LED1 ^= 1; //toggle each loop input
        callback_rocket_state_step(htim);

        /*******************************************************************
         * from here to the end of tim1 callback function
         * is the program that is done every loop
         *
         *
         *******************************************************************/

        /***************************************************
         * update mission time, begin with launch
         ***************************************************/
        if (RocketsVar.Rocket_State >= LAUNCH && RocketsVar.Rocket_State <= LANDING) {
            RocketsVar.Mission_Time += RocketsVar.Main_Loop_Time_Step;
        }

        /***************************************************
         * update de la pression/temperature du barometre
         * et calcul de la densité de l'air
         ***************************************************/
        Compute_Barometer_Pressure(&Barometer);
        Compute_Air_Density(&Barometer);
        Compute_Sound_Speed(&Barometer);

        //simulator
        //overwrite pressure data only
        /*
        if (loop_counter < sizeof(PRESSURE_POINTS_HYPERION) >> 2) {
            Barometer.pressure = PRESSURE_POINTS_HYPERION[loop_counter % sizeof(PRESSURE_POINTS_HYPERION)];
        }
*/

        /*
        if (loop_counter < sizeof(PRESSURE_POINTS_HYPERION_SONIC)) {
            Barometer.pressure = PRESSURE_POINTS_HYPERION_SONIC[loop_counter % sizeof(PRESSURE_POINTS_HYPERION) >> 2];
        }

        if (loop_counter < sizeof(PRESSURE_POINT_AMAROK)>>2) {
            Barometer.pressure = PRESSURE_POINT_AMAROK[loop_counter%sizeof(PRESSURE_POINTS_HYPERION)];
        }
*/

        /***************************************************
         * update de l'altimetre et de kalman
         ***************************************************/
        Altimeter.Barometric_Altitude = Convert_mBar_To_Altitude(
            Barometer.pressure);

        //grab sea altitude level to offset AGL altitude
        if (RocketsVar.Rocket_State == INITIALISATION) {
            while (Altimeter.Initial_Altitude_Count) {
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
         * get_time must be called before get_date
         * see get_date comments
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
            (char*)(Save_String),
            "20%02d-%02d-%02dT%02d:%02d:%02d,%s,%lu,%lu,%lu,%lu,%lu,%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%lu,%lu,%f,%f,%lu,%lu,%lu,%f,%f,%f,%f,%lu,%f,%f,%f,%f,%f,%f,%f\n",
            sDate.Year,
            sDate.Month,
            sDate.Date,
            sTime.Hours,
            sTime.Minutes,
            sTime.Seconds,
            Rocket_State_String,
            RocketsVar.Mission_Time,
            ((RocketsVar.Main_Compute_Time * 100) / RocketsVar.Main_Loop_Time_Step),
            Main_Parachute.Ejection_Charge_Detect,
            Main_Parachute.Ejection_Charge_Fire,
            Drogue_Parachute.Ejection_Charge_Detect,
            Drogue_Parachute.Ejection_Charge_Fire,
            Barometer.temperature,
            Barometer.Air_Density,
            Barometer.Sound_Speed,
            Altimeter.Barometric_Altitude,
            Altimeter.AGL_Altitude,
            Altimeter.Filtered_Altitude,
            Altimeter.Filtered_Velocity,
            Altimeter.Filtered_Acceleration,
            Inertial_Station.bezier_cp1_x,
            Inertial_Station.bezier_cp1_y,
            Inertial_Station.bezier_cp2_x,
            Inertial_Station.bezier_cp2_y,
            Inertial_Station.bezier_fap_x,
            Inertial_Station.bezier_fap_y,
            Inertial_Station.bezier_tip_x,
            Inertial_Station.bezier_tip_y,
            Inertial_Station.motorcmd_left,
            Inertial_Station.motorcmd_right,
            Inertial_Station.motorpos_left,
            Inertial_Station.motorpos_right,
            Inertial_Station.GPS_ground_speed,
            Inertial_Station.GPS_heading_of_motion,
            Inertial_Station.GPS_longitude,
            Inertial_Station.GPS_latitude,
            Inertial_Station.GPS_altitude,
            Inertial_Station.GPS_Fix_Type,
            Inertial_Station.SGP_state,
            Inertial_Station.SGP_horizontal_speed,
            Inertial_Station.SGP_vertical_speed,
            Inertial_Station.SGP_descent_time,
            Inertial_Station.SGP_post_tracking,
            Inertial_Station.MTI_sample_time,
            Inertial_Station.accel_x,
            Inertial_Station.accel_y,
            Inertial_Station.accel_z,
            Inertial_Station.gyro_q1,
            Inertial_Station.gyro_q2,
            Inertial_Station.gyro_q3,
			Inertial_Station.gyro_q4);

        f_puts((TCHAR*)Save_String, &data_file);

        /***************************************************
     * Telemetrie
     ***************************************************/

        if (!(loop_counter % Telemetry.Loop_Step)) {
        	send_data_message(&Telemetry, MSG_ID_ALTIMETER_DATA, Altimeter.AGL_Altitude, Altimeter.Filtered_Altitude, Altimeter.Filtered_Velocity, Altimeter.Filtered_Acceleration);
            send_data_message(&Telemetry, MSG_ID_ROCKET_STATE, RocketsVar.Rocket_State);

            /**********************************************
             * JSON root
             *********************************************/

            /**********************************************
             * add sensor field to root
             *********************************************/

            /**********************************************
             * add altimeter field to sensor
             *********************************************/

            /**********************************************
             * add SGP field to sensor
             *********************************************/
            //send_data_message(Telemetry, MSG_ID_ACCELERATION, Inertial_Station.accel_x, Intertial_Station.accel_y, Intertial_Station.accel_z);
            //send_data_message(Telemetry, MSG_ID_GYRO, Inertial_Station.gyro_yaw, Intertial_Station.gyro_yield, Inertial_Station.gyro_roll);
            //send_data_message(Telemetry, MSG_GPS_METADATA, Inertial_Station.GPS_Fix_Type, Inertial_Station.GPS_N_satellite);
            //send_data_message(Telemetry, MSG_GPS_DATA, Inertial_Station.GPS_longitude, Inertial_Station.GPS_latitude);

            /**********************************************
             * add parachute field to sensor
             *********************************************/
            uint8_t main_parachute = 0;
            uint8_t drogue_parachute = 0;
            main_parachute |= (Main_Parachute.Ejection_Charge_Detect ? 0b00010000 : 0);
            main_parachute |= (Main_Parachute.Ejection_Charge_Fire ? 0b00000001 : 0);

            drogue_parachute |= (Drogue_Parachute.Ejection_Charge_Detect ? 0b00010000 : 0);
            drogue_parachute |= (Drogue_Parachute.Ejection_Charge_Fire ? 0b00000001 : 0);

            send_data_message(&Telemetry, MSG_ID_PARACHUTE_STATE, main_parachute, drogue_parachute);
            /**********************************************
             * add ejection setting field to sensor
             *********************************************/
            send_data_message(&Telemetry, MSG_ID_EJECTION_SETTINGS, Altimeter.Apogee_Ejection_Delay, Altimeter.Ultrasonic_Delay, Altimeter.Main_Ejection_Altitude);
        }

        /***************************************************
     * CANBUS comm
     ***************************************************/

        /*
     CanTx_msg.StdId = CAN_GPS_LONGITUDE_ID;
     CanTx_msg.DLC = 0;
     if(HAL_CAN_Transmit(&hcan2, 1) == HAL_OK){
       LED.Critical_LED3 = 1;
     }
     else{
       LED.Critical_LED3 = 0;
     }


     CanTx_msg.StdId = CAN_GPS_LATITUDE_ID;
     CanTx_msg.DLC = 0;
     if(HAL_CAN_Transmit(&hcan2, 1) == HAL_OK){
       LED.Critical_LED3 = 1;
     }
     else{
       LED.Critical_LED3 = 0;
     }

     CanTx_msg.StdId = CAN_GPS_ALTITUDE_ID;
     CanTx_msg.DLC = 0;
     if(HAL_CAN_Transmit(&hcan2, 1) == HAL_OK){
       LED.Critical_LED3 = 1;
     }
     else{
       LED.Critical_LED3 = 0;
     }
    */

        /***************************************************
     * USB SERIAL COM PORT - programation de l'altimetre
     ***************************************************/

        //code test pour programmer l'horloge et les setting d'ejection
        //utilise un des fonction matlab comprisent dans le
        //dossier critical_gui
        //rx is done elsewhere in usb_cdc_if.c
        if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) {
            volatile int val = 0;
            if (USB_CDC_RX[0] == 'a') {
                sDate.Year = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                //HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                //itoa(sDate.Date, USB_CDC_TX, 10);
                //CDC_Transmit_FS(USB_CDC_TX, strlen(USB_CDC_TX));
            }

            if (USB_CDC_RX[0] == 'b') {
                sDate.Date = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'c') {
                sDate.Month = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'd') {
                sTime.Hours = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'e') {
                sTime.Minutes = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'f') {
                sTime.Seconds = atoi((char*)&USB_CDC_RX[1]);
                HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'g') {
                val = atoi((char*)&USB_CDC_RX[1]);
                Altimeter.Ultrasonic_Delay = (float)val;
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'h') {
                val = atoi((char*)&USB_CDC_RX[1]);
                Altimeter.Apogee_Ejection_Delay = (float)val;
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }

            if (USB_CDC_RX[0] == 'i') {
                val = atoi((char*)&USB_CDC_RX[1]);
                Altimeter.Main_Ejection_Altitude = (float)val;
                USB_CDC_RX[0] = 0;
                USB_CDC_RX[1] = 0;
                USB_CDC_RX[2] = 0;
                USB_CDC_RX[3] = 0;
                USB_CDC_RX[4] = 0;
                USB_CDC_RX[5] = 0;
            }
        }

        /***************************************************
     * update rocket state
     ***************************************************/
        //select next state
        State_Manager(&RocketsVar);
        //toggle motherboard heartbeat with every loop
        if (!(loop_counter % 10)) {
            LED.Status_LED_Proc_Good ^= 1;
        }

        //update gps fix led status
        if (Inertial_Station.GPS_Fix_Type >= 3) {
            LED.Status_LED_GPS_fixed = 1;
        } else {
            LED.Status_LED_GPS_fixed = 0;
        }

        //loop counter incrementation
        loop_counter++;

        /***************************************************
     * Compute main loop time tick-toc
     ***************************************************/
        RocketsVar.Main_Compute_Time = (uint32_t)((((float_t)__HAL_TIM_GET_COUNTER(htim)) * 0.00152592547));

    } //TIm1 callback function end

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

        //save user settings to Backup SRAM
        //this ram sector wont erase upon power cycling
        //if a cmos battery is present
        Set_Backup_Apogee_Delay(&Backup_Settings, Altimeter.Apogee_Ejection_Delay);
        Set_Backup_Altitude_Main(&Backup_Settings,
            Altimeter.Main_Ejection_Altitude);
        Set_Backup_Sonic_Delay(&Backup_Settings, Altimeter.Ultrasonic_Delay);

        Save_Backup_SRAM(&Backup_Settings);

        save_counter++;
    }

    /*********************************************************************************************
   *	TIM3 callback
   *	called by timer handler opon number of mainloop count, clock is tim1 output
   *	value is 20 x 50ms = 1seconde
   *
   *	***not used***
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
 * CAN2 RX callback
 * gere les request de data des autres module et la reception de data de ces modules
 *
 * ARGUMENTS :
 * 		-
 *
 * RETURN :
 * 		-
 *
 *********************************************************************************************/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{

    if (hcan->Instance == CAN2) {
        //toggle led when entering callback
        LED.Critical_LED3 ^= 1;

        switch (hcan->pRxMsg->StdId) {

        case CAN_ROCKET_STATE_REQ_ID:
            CanTx_msg.StdId = CAN_ROCKET_STATE_ID;
            CanTx_msg.DLC = sizeof(Rocket_State_t);
            memcpy(hcan->pTxMsg->Data, &RocketsVar.Rocket_State,
                sizeof(Rocket_State_t));
            HAL_CAN_Transmit(&hcan2, 1);
            break;

        case CAN_ROCKET_ALTITUDE_REQ_ID:
            CanTx_msg.StdId = CAN_ROCKET_ALTITUDE_ID;
            CanTx_msg.DLC = sizeof(Altimeter.AGL_Altitude);
            memcpy(hcan->pTxMsg->Data, &Altimeter.AGL_Altitude,
                sizeof(Altimeter.AGL_Altitude));
            HAL_CAN_Transmit(&hcan2, 1);
            break;

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
            Inertial_Station.GPS_altitude *= 0.001;
            // FIXME: I send the data message here because as seen in the SGP board source code,
            // the three GPS messages are always sent in this order. This is a HACK. Granted,
            // this entire code base is a hack, but I wrote this part and I feel bad.
            send_data_message(&Telemetry, MSG_ID_GPS_DATA, Inertial_Station.GPS_longitude, Inertial_Station.GPS_latitude, Inertial_Station.GPS_altitude);
            break;
        case CAN_GPS_FIX_TYPE_ID:
            memcpy(&Inertial_Station.GPS_Fix_Type, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.GPS_Fix_Type));
            break;
        case CAN_GPS_N_SATELLITE_ID:
            memcpy(&Inertial_Station.GPS_N_satellite, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.GPS_N_satellite));
            send_data_message(&Telemetry, MSG_ID_GPS_METADATA, Inertial_Station.GPS_Fix_Type, Inertial_Station.GPS_N_satellite);
            break;
        case CAN_MTI_SAMPLE_TIME_ID:
            memcpy(&Inertial_Station.MTI_sample_time, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.MTI_sample_time));
            break;
        case CAN_ACCELERATION_X_ID:
            memcpy(&Inertial_Station.accel_x, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.accel_x));
            break;
        case CAN_ACCELERATION_Y_ID:
            memcpy(&Inertial_Station.accel_y, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.accel_y));
            break;
        case CAN_ACCELERATION_Z_ID:
            memcpy(&Inertial_Station.accel_z, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.accel_z));
            send_data_message(&Telemetry, MSG_ID_ACCELERATION, Inertial_Station.accel_x, Inertial_Station.accel_y, Inertial_Station.accel_z);
            break;
        case CAN_GYRO_Q1_ID:
            memcpy(&Inertial_Station.gyro_q1, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.gyro_q1));
            break;
        case CAN_GYRO_Q2_ID:
            memcpy(&Inertial_Station.gyro_q2, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.gyro_q2));
            break;
        case CAN_GYRO_Q3_ID:
            memcpy(&Inertial_Station.gyro_q3, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.gyro_q3));
        case CAN_GYRO_Q4_ID:
			memcpy(&Inertial_Station.gyro_q4, hcan->pRxMsg->Data,
				sizeof(Inertial_Station.gyro_q4));
            send_data_message(&Telemetry, MSG_ID_GYRO, Inertial_Station.gyro_q1, Inertial_Station.gyro_q2, Inertial_Station.gyro_q3, Inertial_Station.gyro_q4);
            break;
        case CAN_BEZIER_CP1_X:
            memcpy(&Inertial_Station.bezier_cp1_x, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_cp1_x));
            break;
        case CAN_BEZIER_CP1_Y:
            memcpy(&Inertial_Station.bezier_cp1_y, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_cp1_y));
            break;
        case CAN_BEZIER_CP2_X:
            memcpy(&Inertial_Station.bezier_cp2_x, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_cp2_x));
            break;
        case CAN_BEZIER_CP2_Y:
            memcpy(&Inertial_Station.bezier_cp2_y, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_cp2_y));
            break;
        case CAN_BEZIER_FAP_X:
            memcpy(&Inertial_Station.bezier_fap_x, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_fap_x));
            break;
        case CAN_BEZIER_FAP_Y:
            memcpy(&Inertial_Station.bezier_fap_y, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_fap_y));
            break;
        case CAN_BEZIER_TIP_X:
            memcpy(&Inertial_Station.bezier_tip_x, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_tip_x));
            break;
        case CAN_BEZIER_TIP_Y:
            memcpy(&Inertial_Station.bezier_tip_y, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.bezier_tip_y));
            break;
        case CAN_MOTORCMD_LEFT:
            memcpy(&Inertial_Station.motorcmd_left, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.motorcmd_left));
            break;
        case CAN_MOTORCMD_RIGHT:
            memcpy(&Inertial_Station.motorcmd_right, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.motorcmd_right));
            break;
        case CAN_MOTORPOS_LEFT:
            memcpy(&Inertial_Station.motorpos_left, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.motorpos_left));
            break;
        case CAN_MOTORPOS_RIGHT:
            memcpy(&Inertial_Station.motorpos_right, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.motorpos_right));
            break;
        case CAN_GPS_HEADING_MOTION_ID:
            memcpy(&Inertial_Station.GPS_heading_of_motion, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.GPS_heading_of_motion));
            Inertial_Station.GPS_heading_of_motion *= 0.00001;
            break;
        case CAN_GPS_GROUND_SPEED_ID:
            memcpy(&Inertial_Station.GPS_ground_speed, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.GPS_ground_speed));
            Inertial_Station.GPS_ground_speed *= 0.00001;
            break;
        case CAN_SGP_STATE:
            memcpy(&Inertial_Station.SGP_state, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.SGP_state));
            break;
        case CAN_SGP_DESCENTTIME:
            memcpy(&Inertial_Station.SGP_descent_time, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.SGP_descent_time));
            break;
        case CAN_SGP_HORZSPEED:
            memcpy(&Inertial_Station.SGP_horizontal_speed, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.SGP_horizontal_speed));
            break;
        case CAN_SGP_VERTSPEED:
            memcpy(&Inertial_Station.SGP_vertical_speed, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.SGP_vertical_speed));
            break;
        case CAN_SGP_POSTTRACKING:
            memcpy(&Inertial_Station.SGP_post_tracking, hcan->pRxMsg->Data,
                sizeof(Inertial_Station.SGP_post_tracking));
            break;
        default:
            // do nothing
            // bad case
            break;
        }

        //prepare for next reception
        __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FMP0);
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{

    if (huart->Instance == USART2) {

        if (RFD900.Wait_for_OK_IT == 1) {

            if ((RFD900.RX[0] = 0x4F) && (RFD900.RX[1] == 0x4B)) { // if = 'O' 'K'
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
 * interrupt for end of DMA xfer on usart2 (RFD900)
 * free tx string and clear tx_busy flag
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART2) {
        process_queue(&Telemetry);
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static int i;
    i++;
    //recall all init process
    //pretty slow, but works quite well
    if (BSP_SD_Init() == 0) {
        f_mount(&fs, (TCHAR const*)SD_Path, 1);
        f_open(&data_file, FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
        f_lseek(&data_file, f_size(&data_file));

        //Log file header
        strcpy((char*)Save_String, DATA_LOG_HEADER);
        f_puts((char*)Save_String, &data_file);

        //column title
        strcpy((char*)Save_String, DATA_LOG_COL_NAME);
        f_puts((char*)Save_String, &data_file);

        //write file to sd
        f_close(&data_file);
    }
}

/**
 * 
 */
void callback_rocket_state_step(TIM_HandleTypeDef* htim)
{
    switch (RocketsVar.Rocket_State) {
    case INITIALISATION:
        callback_step_initialisation(htim);
        break;

    case STANDBY_ON_PAD:
        callback_step_standby();
        break;

    case LAUNCH:
        LED.Critical_LED4 = 1;
        Buzzer.Buzzer_enable = 0;
        Telemetry.Loop_Step = 1; //each 4x main loop
        break;

    case POWERED_ASCENT:
        LED.Critical_LED4 = 0;

        // Détection du burnout apres delay ultrasonic
        if (RocketsVar.Mission_Time > Altimeter.Ultrasonic_Delay) {
            Burnout_Detection(&Altimeter);
        }
        break;

    case ENGINE_BURNOUT:
        LED.Critical_LED4 = 1;

        // Enregistrement du temps au burnout
        Altimeter.Burnout_Time = RocketsVar.Mission_Time;

        break;
    case COASTING_ASCENT:
        LED.Critical_LED4 = 0;

        // Détection d'apogée apres delais supersonic
        if (RocketsVar.Mission_Time > Altimeter.Ultrasonic_Delay) {
            Apogee_Detection(&Altimeter);
        }
        break;

    case APOGEE_REACHED:
        LED.Critical_LED4 = 1;

        // Enregistrement du temps à l'apogée
        Altimeter.Apogee_Time = RocketsVar.Mission_Time;
        break;

    case DROGUE_DEPLOYMENT:
        LED.Critical_LED4 = 1;
        deploy_drogue();
        break;

    // rocket is rapidly descending towards ground
    case DROGUE_DESCENT:
        LED.Critical_LED4 = 0;
        break;

    case MAIN_DEPLOYMENT:
        LED.Critical_LED4 = 1;
        deploy_main();
        break;

    // rocket is slowly descending
    case MAIN_DESCENT:
        LED.Critical_LED4 = 0;
        break;

    // landing is imminent
    case LANDING:
        LED.Critical_LED4 = 1;
        break;

    // rocket is in recovery mode
    // rfd900 goes in slow transmission mode to save power for recovery
    case RECOVERY:
        LED.Critical_LED4 = 0;
        Telemetry.Loop_Step = 100;
        break;

    // rocket has been picked up
    case PICKEDUP:
        LED.Critical_LED4 = 1;
        break;

    // rocket has reached an invalid state. this is not good
    default:
        break;
    } //rocket state behavior end
}

/*******************************************************************
 * initialisation des modules
 
 *******************************************************************/
void callback_step_initialisation(TIM_HandleTypeDef* htim)
{
    // le tim1 est arreté pour laisser le temps...
    // (surtout pour le rfd900 en raison des delais de boot)
    HAL_TIM_Base_Stop_IT(htim);
    LED.Critical_LED4 = 0;

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
    Altimeter.Apogee_Ejection_Delay = Get_Backup_Apogee_Delay(&Backup_Settings);
    Altimeter.Ultrasonic_Delay = Get_Backup_Sonic_Delay(&Backup_Settings);
    Altimeter.Main_Ejection_Altitude = Get_Backup_Altitude_Main(&Backup_Settings);

    //canbus buffer setting
    hcan2.pTxMsg = &CanTx_msg;
    hcan2.pRxMsg = &CanRx_msg;

    //canbus filter settings
    CAN_FilterStruct.FilterIdHigh = 0x0000; /* Upper 16bit filter ID */
    CAN_FilterStruct.FilterIdLow = 0x0000; /* Filter lower 16bit ID */
    CAN_FilterStruct.FilterMaskIdHigh = 0x0000; /* Upper 16bit filter mask */
    CAN_FilterStruct.FilterMaskIdLow = 0x0000; /* Lower 16bit filter mask */
    CAN_FilterStruct.FilterFIFOAssignment = CAN_FILTER_FIFO0; /* Which FIFO will be assigned to filter */
    CAN_FilterStruct.FilterNumber = 14; // 0..27 for CAN2
    CAN_FilterStruct.FilterMode = CAN_FILTERMODE_IDMASK; /* Identifier mask mode */
    CAN_FilterStruct.FilterScale = CAN_FILTERSCALE_32BIT; /* 32bit ID filter */
    CAN_FilterStruct.FilterActivation = ENABLE; /* Enable this filter */
    CAN_FilterStruct.BankNumber = 14; /* Start slave bank filter (?) */

    HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterStruct); /* Initialize filter */
    __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);

    //init de fatfs et de la SD
    if (BSP_SD_Init() == 0) {
        f_mount(&fs, (TCHAR const*)SD_Path, 1);
        f_open(&data_file, FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
        f_lseek(&data_file, f_size(&data_file));

        //Log file header
        strcpy((char*)Save_String, DATA_LOG_HEADER);
        f_puts((char*)Save_String, &data_file);

        //column title
        strcpy((char*)Save_String, DATA_LOG_COL_NAME);
        f_puts((char*)Save_String, &data_file);

        //write file to sd
        f_close(&data_file);
    }

    //start telemetry receive callback
    HAL_UART_Receive_IT(&huart2, (uint8_t*)Telemetry.rx, 2);

    //restart main timer after init
    HAL_TIM_Base_Start_IT(htim);
}

/*******************************************************************
 * waiting on the pad for launch
 *******************************************************************/
void callback_step_standby()
{
    LED.Critical_LED4 = 0;
    Buzzer.Buzzer_enable = 1;

    // Update du Buzzer
    if (Drogue_Parachute.Ejection_Charge_Detect && Main_Parachute.Ejection_Charge_Detect) {
        Buzzer.Buzzer_mode = 3;
        LED.Status_LED_Drogue_Armed = 1;
        LED.Status_LED_Main_Armed = 1;
    } else if (Drogue_Parachute.Ejection_Charge_Detect && !Main_Parachute.Ejection_Charge_Detect) {
        Buzzer.Buzzer_mode = 1;
        LED.Status_LED_Drogue_Armed = 1;
        LED.Status_LED_Main_Armed = 0;
    } else if (!Drogue_Parachute.Ejection_Charge_Detect && Main_Parachute.Ejection_Charge_Detect) {
        Buzzer.Buzzer_mode = 2;
        LED.Status_LED_Drogue_Armed = 0;
        LED.Status_LED_Main_Armed = 1;
    } else {
        Buzzer.Buzzer_mode = 0;
        LED.Status_LED_Drogue_Armed = 0;
        LED.Status_LED_Main_Armed = 0;
    }
}

/*******************************************************************
 * drogue parachute is deploying
 *******************************************************************/
void deploy_drogue()
{
    // delai d'ejection du drogue apres l'apogee
    // enregistrement de l'altitude de deploiement
    if (RocketsVar.Mission_Time > Altimeter.Apogee_Time + Altimeter.Apogee_Ejection_Delay) {
        Drogue_Parachute.Ejection_Charge_Fire = 1;
        Drogue_Parachute.Estimated_Altitude_Deployed = Altimeter.Filtered_Altitude;
        Drogue_Parachute.Barometric_Altitude_Deployed = Altimeter.AGL_Altitude;
    }
}

/*******************************************************************
 * main parachute is deploying
 *******************************************************************/
void deploy_main()
{

    // ejection du main parachute
    // enregistrement de l'altitude de deploiement
    Main_Parachute.Ejection_Charge_Fire = 1;
    Main_Parachute.Estimated_Altitude_Deployed = Altimeter.Filtered_Altitude;
    Main_Parachute.Barometric_Altitude_Deployed = Altimeter.AGL_Altitude;
}
