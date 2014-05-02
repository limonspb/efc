#include "serialhandler.h"

SerialHandler::SerialHandler()
{
}

void SerialHandler::writeDataToPort(QByteArray data)
{
    if (_port.isOpen()){
        int n = data.size();
        int times = 0;
        int totalWrote = 0;

        do{
            qint16 currentWrote = _port.write(data.data() + totalWrote, n-totalWrote);
            if (currentWrote == -1){
                emit writeDataFailed(QString("Error code: %1\n").arg(_port.error()));
                break;
            }
            totalWrote += currentWrote;
            times += 1;
        }while (totalWrote != data.size());
        //std::cout << "write data to port with size " << data.size() << "  TIMES: " << times << std::endl;
    }else{
        emit writeDataFailed("Serial is not connected\n");
    }
}

void SerialHandler::OnQuit()
{
    //close port
    std::cout << "close port" << std::endl;
}

void SerialHandler::OnSetPortCfg(QString portName)
{
    if (_port.isOpen()){
        emit out(_port.portName() + " is already connected. Disconnecting...\n");
        _port.close();
    }
    emit out("Opening port: " + portName + "\n");

    bool portExist = false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.portName().compare(portName, Qt::CaseInsensitive) == 0){
            _port.setPort(info);
            portExist = true;


            bool opened = _port.open(QIODevice::ReadWrite);
            QSerialPort::SerialPortError error = _port.error();

            if (opened){
                bool parity, stopBits, baudRate, dataBits, flowControl;
                parity = _port.setParity(QSerialPort::NoParity);
                stopBits = _port.setStopBits(QSerialPort::OneStop);
                baudRate = _port.setBaudRate(QSerialPort::Baud115200);
                dataBits = _port.setDataBits(QSerialPort::Data8);
                flowControl = _port.setFlowControl(QSerialPort::NoFlowControl);

                //QString xxx = xxx + parity + stopBits + baudRate + dataBits + flowControl;
                emit out(QString("Serial port connected. (Seting serial params: %1 %2 %3 %4 %5)\n")
                         .arg(parity).arg(stopBits).arg(baudRate).arg(dataBits).arg(flowControl));
                //_port.reset();

                connect(&_port, SIGNAL(readyRead()), this, SLOT(readData()));
                connect(&_port, SIGNAL(aboutToClose()), this, SLOT(aboutToClose()));
            }else{                
                emit connectionFailed(QString("port.open() failed. Error code: %1\n").arg(error));
                _port.disconnect();
            }
        }
    }

    if (!portExist){
        emit connectionFailed("Port '" + portName + "' absent.\n");
    }
}

void SerialHandler::readData()
{
    /*
        static QByteArray line;
        static QString str;
        if (_port.canReadLine()){
            line = _port.readLine();
            str = line;
        }
        emit DataFromPort(line);
        */
        QByteArray line;
        line = _port.readAll();
        emit DataFromPort(line);
        //_port.readAll();
        //std::cout << "readData()";
}

void SerialHandler::aboutToClose()
{
    //std::cout << "aboutToClose()\n";
}
