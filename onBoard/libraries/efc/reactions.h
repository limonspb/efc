#ifndef REACTIONS_H
#define REACTIONS_H

#include "commanddata.h"
#include "defines.h"

extern void (* reset_Reaction)(Base_CommandData *);

#ifdef QT_ENABLED
extern void (* debugInfo_Reaction)(Base_CommandData *);

extern void (* rc_Reaction)(Base_CommandData *);
extern void (* eepromReceiver_Reaction)(Base_CommandData *);
extern void (* eepromBasicPids_Reaction)(Base_CommandData *);
extern void (* eepromBasicPidsAdditional_Reaction)(Base_CommandData *);
extern void (* mpu_Reaction)(Base_CommandData *);

extern void (* motor_Reaction)(Base_CommandData *);
#endif

extern void (* calibrateRadio_Reaction)(Base_CommandData *);

extern void (* eeprom1_Reaction)(Base_CommandData *);

extern void (* clearEeprom_Reaction)(Base_CommandData *);

extern void (* setPid_Reaction)(Base_CommandData *);

#endif // REACTIONS_H
