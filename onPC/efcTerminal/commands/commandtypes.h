#ifndef COMMANDTYPES_H
#define COMMANDTYPES_H

enum CommandType{
    ctReset = 0,

    ctDebugInfo = 2,

    ctRc = 4,

    ctCalibrateRadio = 6,
    ctEeprom1 = 7,
    ctEepromReceiver = 8,
    ctEepromBasicPids = 9,
    ctEepromBasicPidsAdditional = 10,
    ctClearEeprom = 11,
    ctMpu = 12,
    ctMotor = 13,
    ctSetPid = 14
};

#endif // COMMANDTYPES_H
