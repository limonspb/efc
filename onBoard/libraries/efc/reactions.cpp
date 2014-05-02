#include "reactions.h"
#include "defines.h"

void (* reset_Reaction)(Base_CommandData *) = NULL;

#ifdef QT_ENABLED
void (* debugInfo_Reaction)(Base_CommandData *) = NULL;

void (* rc_Reaction)(Base_CommandData *) = NULL;
void (* eepromReceiver_Reaction)(Base_CommandData *) = NULL;
void (* eepromBasicPids_Reaction)(Base_CommandData *) = NULL;
void (* eepromBasicPidsAdditional_Reaction)(Base_CommandData *) = NULL;
void (* mpu_Reaction)(Base_CommandData *) = NULL;

void (* motor_Reaction)(Base_CommandData *) = NULL;
#endif


void (* calibrateRadio_Reaction)(Base_CommandData *) = NULL;

void (* eeprom1_Reaction)(Base_CommandData *) = NULL;

void (* clearEeprom_Reaction)(Base_CommandData *) = NULL;

void (* setPid_Reaction)(Base_CommandData *) = NULL;
