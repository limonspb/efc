#include "commandmanager.h"
#include <iostream>

CommandManager::CommandManager()
    :_n(0)
    ,_errorCount(0)
{
    for (quint8 i =0; i < MAX_COMMANDS_STACK_SIZE; i++){
        _commandsToDo[i] = NULL;
    }

    _bufferN = 0;
    for (quint16 i =0; i < MAX_BUFFER_SIZE; i++){
        _buffer[i] = 0;
    }
}

bool CommandManager::pushCommand(char *data, quint32 dataSize)
{
    quint8 commandType = getCommandType(data);

    Base_Command * command = NULL;
    bool ok = false;
    switch (commandType){
    case ctReset:
        //command = new Reset_Command( CommandType(commandType));
        command = new Command<Reset_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    #ifdef QT_ENABLED
    case ctDebugInfo:
        command = new Command<DebugInfo_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctRc:
        command = new Command<Rc_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctEepromBasicPids:
        command = new Command<EepromBasicPids_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctEepromBasicPidsAdditional:
        command = new Command<EepromBasicPidsAdditional_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctEepromReceiver:
        command = new Command<EepromReceiver_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctMpu:
        command = new Command<Mpu_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctMotor:
        command = new Command<Motor_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    #endif
    case ctCalibrateRadio:
        command = new Command<CalibrateRadio_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;

    case ctEeprom1:
        command = new Command<Eeprom1_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;


    case ctClearEeprom:
        command = new Command<ClearEeprom_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    case ctSetPid:
        command = new Command<SetPid_CommandData>( CommandType(commandType));
        ok = command->deserialize(data, dataSize);
        break;
    }


    if (ok == true){
        if (_n < MAX_COMMANDS_STACK_SIZE){
            _commandsToDo[_n] = command;
            _n ++;
        }else{
            if (command != NULL)
                delete _commandsToDo[0];
            for (quint8 i = 0; i<MAX_COMMANDS_STACK_SIZE-1; i++){
                _commandsToDo[i] = _commandsToDo[i+1];
            }
                _commandsToDo[MAX_COMMANDS_STACK_SIZE - 1] = command;
            //TODO: смещение стека при переполнении
        }
    }else{
        _errorCount ++;
        if (command != NULL)
            delete command;
    }

    return ok;
}

void CommandManager::pushData(char *data, quint32 dataSize)
{
    //std::cout << "pushData() dataSize = " << dataSize << "\n";
    if (_bufferN + dataSize > MAX_BUFFER_SIZE){
        quint16 offset = _bufferN + dataSize - MAX_BUFFER_SIZE;

        for (quint16 i=0; i<MAX_BUFFER_SIZE - offset; i++){
            _buffer[i] = _buffer[i + offset];
        }
        _bufferN -= offset;
    }

    memcpy(_buffer + _bufferN, data, dataSize);
    _bufferN += dataSize;

    //std::cout << "BufferSize = " << _bufferN << "\n";
}

void CommandManager::proceedDataBuffer()
{
    qint16 tPos = -1;
    qint16 nPos = -1;

    bool needFurtherProceed = false;
    for (quint16 i = 0; i< _bufferN; i++){
        if (_buffer[i] == '\t'){
            tPos = i;
            break;
        }
    }

    if (tPos > 0){
        memcpy(_buffer, _buffer + tPos, _bufferN - tPos);
        _bufferN -= tPos;
        tPos = 0;
    }

    if (tPos == 0){
        if (_bufferN > 1){
            quint8 internalDataLength = _buffer[1];
            if (internalDataLength > 100){
                memcpy(_buffer, _buffer+1, _bufferN-1);
                _bufferN --;
                needFurtherProceed = true;
            }else if (internalDataLength + 3 > _bufferN){
                needFurtherProceed = false;
            }else if (_buffer[internalDataLength + 2] != '\n'){
                memcpy(_buffer, _buffer+1, _bufferN-1);
                _bufferN --;
                needFurtherProceed = true;
            }else{
                nPos = internalDataLength + 2;
            }
        }
    }


    if (nPos > 0){
        this->pushCommand(_buffer, nPos + 1);
        memcpy(_buffer, _buffer + nPos + 1, _bufferN - nPos - 1);
        _bufferN -= (nPos + 1);
        needFurtherProceed = true;
    }

    if (needFurtherProceed){
        this->proceedDataBuffer();
    }
}

void CommandManager::pop()
{
    while (_n > 0){
        Base_Command* command = _commandsToDo[_n - 1];
        if (command)
        {
            command ->exec();
            delete command;
        }
        _commandsToDo[_n - 1] = NULL;
        _n --;
    }
}

quint8 CommandManager::getCommandType(char *data)
{
    return *(data + 2);
}
