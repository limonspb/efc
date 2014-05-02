#ifndef MEMORY_H
#define MEMORY_H

#include "defines.h"

#define EEPROM_SIZE 100

#pragma pack(push, 1)
struct MemoryData{
    quint32 firstTime;
    quint16 version;
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

    struct BasicPidsData{
        struct PidData{
            qint16 P;
            qint16 I;
            qint16 D;
        } y, p, r;
    } basicPidsData;

    struct BasicPidsAdditionalData{
        struct PidAdditionalData{
            qint16 maxI;
            qint16 globalMultiplier;
        } y, p, r;
    } basicPidsAdditionalData;

};
#pragma pack(pop)

class Memory{
public:
    static Memory & getInstance(){
        static Memory instance;
        return instance;
    };

    void save();
    void load();

    void clear();

    MemoryData & data(){
        return _data;
    };

    void * getDataAddress(){
        return &_data;
    };

private:
    Memory();
    Memory(const Memory &);
    Memory & operator= (const Memory &);

    void writeDefault();
    void setDefault(MemoryData & d);
    MemoryData _data;
};

#endif