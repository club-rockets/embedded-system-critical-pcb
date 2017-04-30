#include "User_Setting_Backup.h"
/*
 AHB1   |   0x4002 4000 - 0x4002 4FFF   |   BKPSRAM
 APB1   |   0x4000 2800 - 0x4000 2BFF   |   RTC & BKP Registers
 */

void Init_Backup_SRAM(Backup_Setting_t * temp_settings) {

  //Enable access to the backup domain
  HAL_PWR_EnableBkUpAccess();

  //Enable the SRAM PWR clock
  __HAL_RCC_BKPSRAM_CLK_ENABLE();
  __HAL_RCC_BKPSRAM_CLK_SLEEP_ENABLE();

  //Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode
  HAL_PWREx_EnableBkUpReg();

  Read_Backup_SRAM(temp_settings);
}

void Save_Backup_SRAM(Backup_Setting_t * temp_settings) {
  memcpy(BACKUP_SRAM_ADDRESS,temp_settings,sizeof(Backup_Setting_t));
}

void Read_Backup_SRAM(Backup_Setting_t * temp_settings) {
  memcpy(temp_settings,BACKUP_SRAM_ADDRESS,sizeof(Backup_Setting_t));
}

float Get_Backup_Apogee_Delay(Backup_Setting_t * temp_settings){
  return temp_settings->drogue_apogee_delay;
}

float Get_Backup_Altitude_Main(Backup_Setting_t * temp_settings){
  return temp_settings->main_altitude_ejection;
}

float Get_Backup_Sonic_Delay(Backup_Setting_t * temp_settings){
  return temp_settings->supersonic_delay;
}

void Set_Backup_Apogee_Delay(Backup_Setting_t * temp_settings, float value){
  temp_settings->drogue_apogee_delay = value;
}

void Set_Backup_Altitude_Main(Backup_Setting_t * temp_settings, float value){
  temp_settings->main_altitude_ejection = value;
}

void Set_Backup_Sonic_Delay(Backup_Setting_t * temp_settings, float value){
  temp_settings->supersonic_delay = value;
}
