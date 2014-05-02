#include "commands.h"

Base_Command::Base_Command(CommandType type)
    :_reaction(NULL)
{
    _type = type;    

    switch (_type){
    case ctReset:
        this->_reaction = reset_Reaction;
        break;
    #ifdef QT_ENABLED
    case ctDebugInfo:
        this->_reaction = debugInfo_Reaction;
        break;
    case ctRc:
        this->_reaction = rc_Reaction;
        break;
    case ctEepromReceiver:
        this->_reaction = eepromReceiver_Reaction;
        break;
    case ctEepromBasicPids:
        this->_reaction = eepromBasicPids_Reaction;
        break;
    case ctEepromBasicPidsAdditional:
        this->_reaction = eepromBasicPidsAdditional_Reaction;
        break;
    case ctMpu:
        this->_reaction = mpu_Reaction;
        break;
    case ctMotor:
        this->_reaction = motor_Reaction;
        break;
    #endif
    case ctCalibrateRadio:
        this->_reaction = calibrateRadio_Reaction;
        break;

    case ctEeprom1:
        this->_reaction = eeprom1_Reaction;
        break;
    case ctClearEeprom:
        this->_reaction = clearEeprom_Reaction;
        break;
    case ctSetPid:
        this->_reaction = setPid_Reaction;
        break;
    }
}

void Base_Command::exec()
{
    if (_reaction != NULL){
        Base_CommandData * data = this->getData();
        _reaction(data);
    }
}


/*
Reset_Command::Reset_Command(CommandType type)
    : Base_Command(type)
{

}

bool Reset_Command::deserialize(char *data, quint32 dataSize)
{
    bool result = false;
    if (dataSize == sizeof(Reset_CommandData)+2){
        memcpy(&_data, data+1, dataSize);

        if (quint8(_type) == _data.type){
            result = true;
        }
    }
    return result;
}

void Reset_Command::serialize(char *data, quint32 &dataSize)
{
    memcpy(data+1, &_data, sizeof(Reset_CommandData));
    data[sizeof(Reset_CommandData)] = '\n';
    data[0] = '\t';
    dataSize = sizeof(Reset_CommandData) + 2;
}

quint32 Reset_Command::serializedSize()
{
    return sizeof(Reset_CommandData)  + 2;
}

Base_CommandData * Reset_Command::getData()
{
    Reset_CommandData * result = new Reset_CommandData();
    memcpy(result, &_data, sizeof(Reset_CommandData));
    return result;
}


TestNumber_Command::TestNumber_Command(CommandType type)
    : Base_Command(type)
{

}

bool TestNumber_Command::deserialize(char *data, quint32 dataSize)
{
    bool result = false;
    if (dataSize == sizeof(TestNumber_CommandData)+2){
        memcpy(&_data, data+1, dataSize);

        if (quint8(_type) == _data.type){
            result = true;
        }
    }
    return result;
}

void TestNumber_Command::serialize(char *data, quint32 &dataSize)
{
    memcpy(data+1, &_data, sizeof(TestNumber_CommandData));
    data[sizeof(TestNumber_CommandData)] = '\n';
    data[0] = '\t';
    dataSize = sizeof(TestNumber_CommandData) + 2;
}

quint32 TestNumber_Command::serializedSize()
{
    return sizeof(TestNumber_CommandData) + 2;
}

Base_CommandData * TestNumber_Command::getData()
{
    TestNumber_CommandData * result = new TestNumber_CommandData();
    memcpy(result, &_data, sizeof(TestNumber_CommandData));
    return result;
}
*/
