#include "terminalhandler.h"

TerminalInHandler::TerminalInHandler()    
    : _in(NULL)
{
    _in = new QTextStream(stdin);

}

TerminalInHandler::~TerminalInHandler()
{
    if (_in != NULL){
        delete _in;
    }
}

void TerminalInHandler::Execute()
{
    QString str;
    while (true)

    {
        str = _in->readLine();
        emit NewCmd(str);
    }
}
