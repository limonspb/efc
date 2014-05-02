#include <QCoreApplication>
#include <iostream>


#include <QDebug>
#include <QThread>

#include "dispatcher.h"
#include "terminalhandler.h"
#include "serialhandler.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TerminalInHandler in;

    Dispatcher dispatcher;

    SerialHandler port;
    QThread TerminalInThread;
    QObject::connect(&TerminalInThread, SIGNAL(started()), &in, SLOT(Execute()));
    //QObject::connect(&in, SIGNAL(Quit()), &TerminalInThread, SLOT(terminate()));
    //QObject::connect(&in, SIGNAL(Quit()), &port, SLOT(OnQuit()));
    //QObject::connect(&in, SIGNAL(Quit()), &a, SLOT(quit()));

    QObject::connect(&dispatcher, SIGNAL(OpenPort(QString)), &port, SLOT(OnSetPortCfg(QString)));

    in.moveToThread(&TerminalInThread);

    QObject::connect(&in, SIGNAL(NewCmd(QString)), &dispatcher, SLOT(OnNewCmd(QString)));
    QObject::connect(&dispatcher, SIGNAL(WriteToPort(QByteArray)), &port, SLOT(writeDataToPort(QByteArray)));
    QObject::connect(&port, SIGNAL(DataFromPort(QByteArray)), &dispatcher, SLOT(OnDataFromPort(QByteArray)));


    QObject::connect(&port, SIGNAL(out(QString)), &dispatcher, SLOT(out(QString)));
    QObject::connect(&port, SIGNAL(connectionFailed(QString)), &dispatcher, SLOT( onSerialConnectionFailed(QString) ));
    QObject::connect(&port, SIGNAL(writeDataFailed(QString)), &dispatcher, SLOT( onSerialWriteDataFailed(QString) ));

    TerminalInThread.start();

    return a.exec();
}
