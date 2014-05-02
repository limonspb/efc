#include "dispatcher.h"
#include <QtSerialPort/QSerialPortInfo>

Dispatcher * lastCreatedDispatcher = NULL;


void reset_Reaction_r(Base_CommandData * baseData){
    Reset_CommandData * data = (Reset_CommandData *)(baseData);
    std::cout << "reset_Reaction_r() " << qint32(data->tag) << " " << qint32(data->type) << "\n";
    std::cout.flush();
}

void debugInfo_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->debugInfo_Reaction_d(baseData);
}

void rc_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->rc_Reaction_d(baseData);
}


void calibrateRadio_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->calibrateRadio_Reaction_d(baseData);
}

void eeprom1_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->eeprom1_Reaction_d(baseData);
}

void eepromReceiver_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->eepromReceiver_Reaction_d(baseData);
}

void eepromBasicPids_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->eepromBasicPids_Reaction_d(baseData);
}

void eepromBasicPidsAdditional_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->eepromBasicPidsAdditional_Reaction_d(baseData);
}

void clearEeprom_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->clearEeprom_Reaction_d(baseData);
}

void mpu_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->mpu_Reaction_d(baseData);
}

void motor_Reaction_r(Base_CommandData * baseData){
    lastCreatedDispatcher->motor_Reaction_d(baseData);
}


Dispatcher::Dispatcher()

    : _printDebugInfo(false)
    , _printRc(false)
    , _printPureRc(false)
    , _printMpu(false)
    , _printMotors(false)
    , _printErrors(false)
{    
    out(QString(
            "**********************************************\n"
            "**                                          **\n"
            "**             EfcTerminal v 0.0.1          **\n"
            "**              Type 'h' for help           **\n"
            "**                                          **\n"
            "**********************************************\n"
            ));

    reset_Reaction = reset_Reaction_r;
    debugInfo_Reaction = debugInfo_Reaction_r;
    rc_Reaction = rc_Reaction_r;

    calibrateRadio_Reaction = calibrateRadio_Reaction_r;

    eeprom1_Reaction = eeprom1_Reaction_r;
    eepromBasicPids_Reaction = eepromBasicPids_Reaction_r;
    eepromBasicPidsAdditional_Reaction = eepromBasicPidsAdditional_Reaction_r;
    eepromReceiver_Reaction = eepromReceiver_Reaction_r;


    clearEeprom_Reaction = clearEeprom_Reaction_r;

    mpu_Reaction = mpu_Reaction_r;

    motor_Reaction = motor_Reaction_r;    

    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    _timer.start(0);

    lastCreatedDispatcher = this;
}

void Dispatcher::debugInfo_Reaction_d(Base_CommandData *baseData)
{
    if (_printDebugInfo || _printErrors){
        DebugInfo_CommandData * data = (DebugInfo_CommandData *)(baseData);
        QString str = "";
        if (_printDebugInfo){
            str += QString("Free RAM: %1 bytes").arg(data->freeRam) + "\t";
            str += QString("%1").arg(qint32(data->workTime)) + "\t";
            str += QString("%1").arg(qint32(data->fullIterationPeriod)) + "\t";
            str += QString("%1").arg(qint32(data->iterationPeriod)) + "\t";

            if (data->armed){
                str += "ARMED! ";
            }else{
                str += "disarmed. ";
            }

            if (data->fullFailSafe){
                str += "FULL FAILSAFE!";
            }else{
                str += "no failsafe";
            }

            str+="\n";
        }

        if (_printErrors){
            str+= QString("Fifo errors: %1\t Serial errors: %2\t fifo overflows: %3\n")
                    .arg(data->fifoErrorCount)
                    .arg(data->serialErrorCount)
                    .arg(data->fifoOverflowCount);
        }

        //lastCreatedDispatcher->out(str);
        out(str);
    }
}

void Dispatcher::rc_Reaction_d(Base_CommandData *baseData)
{
    if (_printRc){
        Rc_CommandData * data = (Rc_CommandData *)(baseData);
        QString str = "";
        str += QString("proc: tr: %1   y: %2   p: %3   r: %4\n")
                .arg(data->processed.tr).arg(data->processed.y).arg(data->processed.p).arg(data->processed.r);
        out(str);
    }else if (_printPureRc){
        Rc_CommandData * data = (Rc_CommandData *)(baseData);
        QString str = "";
        str += QString("pure: tr: %1   y: %2   p: %3   r: %4\n")
                .arg(data->pure.tr).arg(data->pure.y).arg(data->pure.p).arg(data->pure.r);
        out(str);
    }
}


void Dispatcher::calibrateRadio_Reaction_d(Base_CommandData *baseData)
{
    CalibrateRadio_CommandData * data = (CalibrateRadio_CommandData *)(baseData);
    QString str = "Calibrated: ";
    if (data->error == 0){
        str += "OK";
    }else{
        str += QString("Error: %1").arg(data->error) ;
    }
    str += '\n';
    out(str);
}

void Dispatcher::eeprom1_Reaction_d(Base_CommandData *baseData)
{
    Eeprom1_CommandData * data = (Eeprom1_CommandData *)(baseData);
    QString str = "EEPROM:\n";

    str += QString("\tFirstTime:%1\n").arg(data->firstTime);
    str += QString("\tVersion:  %1\n").arg(data->version);
    out(str);
}

void Dispatcher::eepromReceiver_Reaction_d(Base_CommandData *baseData)
{
    EepromReceiver_CommandData * data = (EepromReceiver_CommandData *)(baseData);
    QString str = "EEPROM Receiver:\n";


    str += QString("\ttr:       %1\t%2\n").arg(data->rc.minTr).arg(data->rc.maxTr);
    str += QString("\ty:        %1\t%2\n").arg(data->rc.minY).arg(data->rc.maxY);
    str += QString("\tp:        %1\t%2\n").arg(data->rc.minP).arg(data->rc.maxP);
    str += QString("\tr:        %1\t%2\n").arg(data->rc.minR).arg(data->rc.maxR);
    //str += QString("\ta1:       %1\t%2\n").arg(data->rc.minA1).arg(data->rc.maxA1);
    //str += QString("\ta2:       %1\t%2\n").arg(data->rc.minA2).arg(data->rc.maxA2);

    str += QString("\tFSvalue:  %1\n").arg(data->rc.fsValue);

    out(str);
}

void Dispatcher::eepromBasicPids_Reaction_d(Base_CommandData *baseData)
{
    EepromBasicPids_CommandData * data = (EepromBasicPids_CommandData *)(baseData);
    QString str = "EEPROM PIDs:\n";

    str += QString("\tY PID: \t%1\t%2\t%3\n").arg(data->basicPidsData.y.P).arg(data->basicPidsData.y.I).arg(data->basicPidsData.y.D);
    str += QString("\tP PID: \t%1\t%2\t%3\n").arg(data->basicPidsData.p.P).arg(data->basicPidsData.p.I).arg(data->basicPidsData.p.D);
    str += QString("\tR PID: \t%1\t%2\t%3\n").arg(data->basicPidsData.r.P).arg(data->basicPidsData.r.I).arg(data->basicPidsData.r.D);

    out(str);
}

void Dispatcher::eepromBasicPidsAdditional_Reaction_d(Base_CommandData *baseData)
{
    EepromBasicPidsAdditional_CommandData * data = (EepromBasicPidsAdditional_CommandData *)(baseData);
    QString str = "EEPROM PIDs additional data:\n";

    str += QString("\ty: maxI: %1\tglobalMultiplier: %2\n").arg(data->basicPidsData.y.maxI).arg(data->basicPidsData.y.globalMultiplier);
    str += QString("\tp: maxI: %1\tglobalMultiplier: %2\n").arg(data->basicPidsData.p.maxI).arg(data->basicPidsData.p.globalMultiplier);
    str += QString("\tr: maxI: %1\tglobalMultiplier: %2\n").arg(data->basicPidsData.r.maxI).arg(data->basicPidsData.r.globalMultiplier);

    out(str);
}


void Dispatcher::clearEeprom_Reaction_d(Base_CommandData *baseData)
{
    ClearEeprom_CommandData * data = (ClearEeprom_CommandData *)(baseData);
    QString str = "EEPROM cleared: ";
    if (data->error == 0){
        str += "OK";
    }else{
        str += QString("Error: %1").arg(data->error) ;
    }
    str += '\n';
    out(str);

}

void Dispatcher::mpu_Reaction_d(Base_CommandData *baseData)
{
    if (_printMpu){
        Mpu_CommandData * data = (Mpu_CommandData *)(baseData);
        QString str = "";
        str+=QString("y: %1\tp: %2\tr: %3\t").arg(data->y).arg(data->p).arg(data->r);
        //str+=QString("gX: %1\t gY: %2\t gZ: %3").arg(data->gx).arg(data->gy).arg(data->gz);
        str += '\n';
        out(str);
    }
}

void Dispatcher::motor_Reaction_d(Base_CommandData *baseData)
{
    if (_printMotors){
        Motor_CommandData * data = (Motor_CommandData *)(baseData);
        QString str = "";
        qreal mult = 100;

        str+=QString("fl: %1\tfr: %2\tbl: %3\tbr: %4")
                .arg(qint32(data->fl * mult))
                .arg(qint32(data->fr * mult))
                .arg(qint32(data->bl * mult))
                .arg(qint32(data->br * mult));

        //str+=QString("fl: %1\tfr: %2").arg(data->bl * mult).arg(data->br * mult);
//        str+=QString("fl: %1\tfr: %2\tbl: %3\tbr: %4")
//                .arg(qint32(data->br * mult));
        str += '\n';
        out(str);
    }
}



void Dispatcher::OnNewCmd(const QString &cmd)
{
    QString clearCmd = cmd.trimmed().toLower();

    static const QString CmdWriteToPort = "w";



    //std::cout << "command received: " << cmd.toStdString().c_str() << std::endl;

    if (clearCmd == "ls" ||
        clearCmd == "ports" ||
        clearCmd == "list")
    {
        this->outPortList();
    }

    if (clearCmd.startsWith("connect ") ||
        clearCmd.startsWith("c "))
    {
        this->tryToConnect(clearCmd);
    }

    if (clearCmd == "debug"){
        _printDebugInfo = true;
    }
    if (clearCmd == "rc"){
        _printPureRc = false;
        _printRc = true;
    }
    if (clearCmd == "rc pure"){
        _printPureRc = true;
        _printRc = false;
    }
    if (clearCmd == "mpu"){
        _printMpu = true;
    }
    if (clearCmd == "motors"){
        _printMotors = true;
    }

    if (clearCmd == "errors"){
        _printErrors = true;
    }
    if (clearCmd == "armstatus"){
/*        Command<ArmStatus_CommandData> c(ctArmStatus);
        quint32 size = c.serializedSize();
        char * data = new char[c.serializedSize()];
        c.data().type = quint8(ctArmStatus);
        c.data().armed = 0;

        c.serialize(data, size);
        QByteArray ba(data, size);


        emit WriteToPort(ba);
        delete data;*/
    }

    if (clearCmd == "eeprom clear"){
        Command<ClearEeprom_CommandData> c(ctClearEeprom);
        quint32 size = c.serializedSize();
        char * data = new char[c.serializedSize()];
        //c.data().type = quint8(ctClearEeprom);
        c.data().error = 0;

        c.serialize(data, size);
        QByteArray ba(data, size);

        //CommandManager::getInstance().pushCommand(data, size);
        emit WriteToPort(ba);
        delete data;
    }

    if (clearCmd == "eeprom get"){
        Command<Eeprom1_CommandData> c(ctEeprom1);
        quint32 size = c.serializedSize();
        char * data = new char[c.serializedSize()];
        //c.data().type = quint8(ctEeprom1);

        c.serialize(data, size);
        QByteArray ba((char*)c.msg(), size);

        //CommandManager::getInstance().pushData(data, 15);
        //CommandManager::getInstance().proceedDataBuffer();
        //CommandManager::getInstance().pushData(data+15, size-15);
        //CommandManager::getInstance().proceedDataBuffer();
        emit WriteToPort(ba);
        delete data;
    }

    if (clearCmd.startsWith("setpid")){
        clearCmd.remove(0, QString("setpid ").length());
        QStringList list = clearCmd.split(" ");
        bool ok = false;
        if (list.count() == 2){
            bool conversion1Ok = false;
            bool conversion2Ok = false;
            qint16 offset = list.at(0).toInt(&conversion1Ok);
            qint16 value = list.at(1).toInt(&conversion2Ok);
            if (conversion1Ok && conversion2Ok){
                Command<SetPid_CommandData> c(ctSetPid);
                c.data().offset = offset;
                c.data().value = value;
                ok = true;
                QByteArray ba((char*)c.msg(), c.serializedSize());
                emit WriteToPort(ba);
            }
        }

        if (!ok){
            out("wrong SetPid parameters\n");
        }
    }

    if (clearCmd == "rc calibrate"){
        Command<CalibrateRadio_CommandData> c(ctCalibrateRadio);
        quint32 size = c.serializedSize();
        char * data = new char[c.serializedSize()];
        //c.data().type = quint8(ctCalibrateRadio);

        c.serialize(data, size);
        QByteArray ba(data, size);

        //CommandManager::getInstance().pushCommand(data, size);
        out("Starting calibration...\n");
        emit WriteToPort(ba);
        delete data;
    }


    if (clearCmd == "s"){
        this->stopAllOutput();
    }


    if (cmd == CmdWriteToPort)
    {
        emit WriteToPort("kjdflkjdfkljghkdfhgkhsdfhglsdkhf");
    }

    if (cmd == "port"){
        emit OpenPort(QString("COM6"));
    }

    if (cmd.startsWith("pw ", Qt::CaseInsensitive)){
        QString str = cmd;
        str.remove(QString("pw "), Qt::CaseInsensitive);
        QByteArray ba;
        ba.append(str.trimmed().toLocal8Bit());
        emit WriteToPort(ba);
    }

    if (cmd == "reset"){
        Command<Reset_CommandData> c(ctReset);
        c.data().tag = 0;

        QByteArray ba((char *)c.msg(), c.serializedSize());

        emit WriteToPort(ba);
        out("Reseting...\n");
        //CommandManager::getInstance().pushCommand(data,4);
    }
    if (cmd == "test"){
        Command<DebugInfo_CommandData> c(ctDebugInfo);
        quint32 size = c.serializedSize();
        char * data = new char[size];

        c.data().type = 2;
        c.data().workTime = 101;
        c.data().iterationPeriod = 50;
        c.data().fullIterationPeriod = 200;

        c.serialize(data, size);

        //char * garbage = new char[5];
        //garbage[0] = 'c';
        //garbage[1] = 'a';
        //garbage[2] = 'b';
        //garbage[3] = 'd';
        //garbage[4] = '5';


        //CommandManager::getInstance().pushCommand(data,size);
        //CommandManager::getInstance().pushData(garbage, 4);
        //CommandManager::getInstance().pushData(data, size);
        //CommandManager::getInstance().pushData(garbage, 2);
        //CommandManager::getInstance().pushData(data, size);
        //CommandManager::getInstance().pushData(garbage, 5);
        //CommandManager::getInstance().pushData(data, size);
        //CommandManager::getInstance().pushData(garbage, 5);
        CommandManager::getInstance().pushData(data, size);
        //CommandManager::getInstance().pushData(garbage, 5);
        CommandManager::getInstance().proceedDataBuffer();
    }
}

void Dispatcher::OnDataFromPort(QByteArray portData)
{
    //std::cout << "data from port received. size: " << portData.size() << std::endl;
    CommandManager::getInstance().pushData(portData.data(), portData.size());
    //std::cout << "_bufferN 1 = " << CommandManager::getInstance()._bufferN << "\n";
    CommandManager::getInstance().proceedDataBuffer();
    //std::cout << "_bufferN 2 = " << CommandManager::getInstance()._bufferN << "\n";


//    out(portData);
    //for (int i=0; i<portData.size(); i++){
    //    out(QString("%1").arg(i) + ": " + portData.at(i) + "\n");
    //}

}

void Dispatcher::out(QString str)
{
    std::cout << str.toStdString().c_str();
}

void Dispatcher::onSerialConnectionFailed(QString reason)
{
    out("Serial connection failed. Reason:\n\t" + reason);
}

void Dispatcher::onSerialWriteDataFailed(QString reason)
{
    out("Serial data writing failed. Reason:\n\t" + reason);
}

void Dispatcher::onTimer()
{
    //out("timer\n");
    CommandManager::getInstance().pop();
}

void Dispatcher::outPortList()
{
    qint32 i =0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        i++;
        out(QString("%1.\t%2\n\t%3, %4\n").arg(i).arg(info.portName(), info.description(), info.manufacturer()));
    }
}

void Dispatcher::tryToConnect(QString cmd)
{
    if (cmd.startsWith("connect ")){
        cmd.remove(0, QString("connect ").length());
    }else if (cmd.startsWith("c ")){
        cmd.remove(0, QString("c ").length());
    }else{
        out("Error AX001 tryToConnect()\n");
        return;
    }
    emit OpenPort(cmd);


}

void Dispatcher::stopAllOutput()
{
    _printDebugInfo = false;
    _printRc = false;
    _printPureRc = false;
    _printMpu = false;
    _printMotors = false;
    _printErrors = false;
}
