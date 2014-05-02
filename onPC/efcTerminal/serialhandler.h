#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <iostream>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>

class SerialHandler : public QObject
{
    Q_OBJECT

public:
    SerialHandler();
signals:
    void DataFromPort(QByteArray);
    void connectionFailed(QString reason);
    void writeDataFailed(QString reason);
    void out(QString str);
public slots:

    void writeDataToPort(QByteArray data);

    void OnQuit();

    void OnSetPortCfg(QString portName);

    void readData();

    void aboutToClose();

private:
    QSerialPort _port;
};
#endif // SERIALHANDLER_H
