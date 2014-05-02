#include "memory.h"

#include <EEPROM.h>
#include "configs.h"

Memory::Memory(){
    load();
}

void Memory::writeDefault(){
    this->setDefault(_data);
}

void Memory::setDefault(MemoryData & d){
    d.firstTime = FLAG_FIRST_TIME;
    d.version = MEM_VERSION;

    /*d.rc.maxA1 = d.rc.maxA2 = */d.rc.maxP = d.rc.maxR = d.rc.maxTr = d.rc.maxY = DEFAULT_RC_MAX;
    /*d.rc.minA1 = d.rc.minA2 = */d.rc.minP = d.rc.minR = d.rc.minTr = d.rc.minY = DEFAULT_RC_MIN;

    d.rc.fsValue = DEFAULT_FS_VALUE;

    d.basicPidsData.y.P = d.basicPidsData.p.P = d.basicPidsData.r.P = DEFAULT_P_VALUE;
    d.basicPidsData.y.I = d.basicPidsData.p.I = d.basicPidsData.r.I = DEFAULT_I_VALUE;
    d.basicPidsData.y.D = d.basicPidsData.p.D = d.basicPidsData.r.D = DEFAULT_D_VALUE;

    d.basicPidsAdditionalData.y.maxI = d.basicPidsAdditionalData.p.maxI = d.basicPidsAdditionalData.r.maxI = DEFAULT_PID_MAX_I;
    d.basicPidsAdditionalData.y.globalMultiplier = d.basicPidsAdditionalData.p.globalMultiplier = d.basicPidsAdditionalData.r.globalMultiplier = DEFAULT_PID_GLOBAL_MULTIPLIER;
}

void Memory::save(){
    for(qint16 i =0; i < sizeof(MemoryData); i++){
        EEPROM.write(i, *((char *)(&_data)+i) );
    }
}

void Memory::load(){
    for(qint16 i =0; i < sizeof(MemoryData); i++){
        *((char *)(&_data)+i) = EEPROM.read(i);
    }
    if (FLAG_FIRST_TIME != _data.firstTime){
        setDefault(_data);
        save();
    }
}

void Memory::clear(){
    for (int i = 0; i < EEPROM_SIZE; i++){
        EEPROM.write(i, 0);
    }
    //setDefault(_data);
    //save();
    this->load();
}





