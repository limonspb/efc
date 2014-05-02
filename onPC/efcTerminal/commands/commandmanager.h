#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include "commands.h"
#include "defines.h"
#include "commandtypes.h"
#include "commanddata.h"

#ifdef QT_ENABLED
    #define MAX_COMMANDS_STACK_SIZE 8
    #define MAX_BUFFER_SIZE 1024
#else
    #define MAX_COMMANDS_STACK_SIZE 3
    #define MAX_BUFFER_SIZE 256
#endif

class CommandManager
{
protected:
    CommandManager();
    CommandManager( const CommandManager& );
    CommandManager& operator=( CommandManager& );

public:

    static CommandManager & getInstance()
    {
        static CommandManager instance;
        return instance;
    }

    bool pushCommand(char * data, quint32 dataSize);
    void pushData(char * data, quint32 dataSize);
    void proceedDataBuffer();
    void pop();
    quint16 _bufferN;
    quint8 errorCount(){
        return _errorCount;
    }

protected:
    quint8 _errorCount;
    quint8 getCommandType(char * data);

    char _buffer[MAX_BUFFER_SIZE];


    Base_Command * _commandsToDo[MAX_COMMANDS_STACK_SIZE];
    quint8 _n;
};

#endif // COMMANDMANAGER_H
