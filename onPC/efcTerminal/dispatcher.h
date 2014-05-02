#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QObject>
#include <iostream>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

#include "commands/commands_include.h"

class Dispatcher : public QObject
{
    Q_OBJECT

public:
    Dispatcher();

    void debugInfo_Reaction_d(Base_CommandData * baseData);
    void rc_Reaction_d(Base_CommandData * baseData);

    void calibrateRadio_Reaction_d(Base_CommandData * baseData);

    void eeprom1_Reaction_d(Base_CommandData * baseData);
    void eepromReceiver_Reaction_d(Base_CommandData * baseData);
    void eepromBasicPids_Reaction_d(Base_CommandData * baseData);
    void eepromBasicPidsAdditional_Reaction_d(Base_CommandData * baseData);

    void clearEeprom_Reaction_d(Base_CommandData * baseData);

    void mpu_Reaction_d(Base_CommandData * baseData);

    void motor_Reaction_d(Base_CommandData * baseData);


public slots:
    void OnNewCmd(const QString& cmd);

    void OnDataFromPort(QByteArray portData);


    void out(QString str);
    void onSerialConnectionFailed(QString reason);
    void onSerialWriteDataFailed(QString reason);
signals:

    void WriteToPort(const QByteArray&);
    void OpenPort(QString portName);

protected slots:
    void onTimer();

protected:

    QTimer _timer;
    void outPortList();
    void tryToConnect(QString cmd);

    bool _printDebugInfo;
    bool _printRc;
    bool _printPureRc;
    bool _printMpu;
    bool _printMotors;
    bool _printErrors;
    void stopAllOutput();
private:    
    QTextStream * _out;
};

#endif // DISPATCHER_H
