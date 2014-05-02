#ifndef COMMANDDATA_H
#define COMMANDDATA_H

#include "defines.h"


#pragma pack(push, 1)

struct Base_CommandData{
    qint8 type;
};

struct Reset_CommandData : Base_CommandData{
    qint8 tag;
};

struct DebugInfo_CommandData : Base_CommandData{
    quint32 workTime; //chip work time in msecs
    quint16 iterationPeriod;
    quint16 fullIterationPeriod;

    quint8 armed;
    quint8 fullFailSafe;

    quint32 freeRam;
    quint8 fifoErrorCount;
    quint8 serialErrorCount;
    quint8 fifoOverflowCount;
};

struct Rc_CommandData : Base_CommandData{
    struct {
        quint16 tr;
        quint16 y;
        quint16 p;
        quint16 r;
        //quint16 a1;
        //quint16 a2;
    } pure, processed;
};

struct CalibrateRadio_CommandData : Base_CommandData{
    quint8 error;
};

struct Eeprom1_CommandData : Base_CommandData{
    quint32 firstTime;
    quint16 version;
};
struct EepromReceiver_CommandData : Base_CommandData{

    struct ReceiverData{
        quint16 minTr;
        quint16 maxTr;
        quint16 minY;
        quint16 maxY;
        quint16 minP;
        quint16 maxP;
        quint16 minR;
        quint16 maxR;
//        quint16 minA2;
//        quint16 maxA1;
//        quint16 minA1;
//        quint16 maxA2;

        quint16 fsValue;
    } rc;
};

struct EepromBasicPids_CommandData : Base_CommandData{
    struct BasicPidsData{
        struct PidData{
            qint16 P;
            qint16 I;
            qint16 D;
        } y, p, r;
    } basicPidsData;
};

struct EepromBasicPidsAdditional_CommandData : Base_CommandData{
    struct BasicPidsAdditionalData{
        struct PidAdditionalData{
            quint16 maxI;
            quint16 globalMultiplier;
        } y, p, r;
    } basicPidsData;
};



struct ClearEeprom_CommandData : Base_CommandData{
    quint8 error;
};

struct Mpu_CommandData : Base_CommandData{
    float y;
    float p;
    float r;

    float gx;
    float gy;
    float gz;
};

struct Motor_CommandData : Base_CommandData{
    float fl;
    float fr;
    float bl;
    float br;
};

struct SetPid_CommandData : Base_CommandData{
    qint16 offset;
    qint16 value;
};



#pragma pack(pop)

#endif // COMMANDDATA_H
