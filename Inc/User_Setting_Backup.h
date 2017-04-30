
#include "string.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_rcc.h"




#define BACKUP_SRAM_ADDRESS ((unsigned long *) 0x40024000)


typedef struct Backup_Setting_s {
  float drogue_apogee_delay;
  float main_altitude_ejection;
  float supersonic_delay;
} Backup_Setting_t;


void Init_Backup_SRAM(Backup_Setting_t * temp_settings);
void Save_Backup_SRAM(Backup_Setting_t * temp_settings);
void Read_Backup_SRAM(Backup_Setting_t * temp_settings);

float Get_Backup_Apogee_Delay(Backup_Setting_t * temp_settings);
float Get_Backup_Altitude_Main(Backup_Setting_t * temp_settings);
float Get_Backup_Sonic_Delay(Backup_Setting_t * temp_settings);

void Set_Backup_Apogee_Delay(Backup_Setting_t * temp_settings, float value);
void Set_Backup_Altitude_Main(Backup_Setting_t * temp_settings, float value);
void Set_Backup_Sonic_Delay(Backup_Setting_t * temp_settings, float value);

