//#include <string>
#include <Arduino.h>

#include "EfRc.h"
#include "receiver.h"
#include "commands_include.h"
#include "memory.h"
#include <EEPROM.h>
#include <math.h>

#include <Servo.h>

#include "Motors.h"

#include "helper_3dmath.h"

#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
//#include "MPU6050.h" // not necessary if using MotionApps include file
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "Sensor.h"

#include "configs.h"

//#include <avr/wdt.h>

//#define sensorRef Sensor::getInstance()
//#define receiverRef Receiver::getInstance()
#define motorsRef Motors::getInstance()
#define commandManagerRef CommandManager::getInstance()

Sensor & sensorRef = Sensor::getInstance();
Receiver & receiverRef = Receiver::getInstance();
//Motors & motorsRef = Motors::getInstance();
//CommandManager & commandManagerRef = CommandManager::getInstance();

void outPids(){    
    Command<EepromBasicPids_CommandData> c2(ctEepromBasicPids);
    memcpy((char *)(&c2.data()) + 1, &Memory::getInstance().data().basicPidsData, sizeof(c2.data())-1 );    
    Serial.write((quint8 *)c2.msg(), c2.serializedSize());
    Serial.flush();
    Command<EepromBasicPidsAdditional_CommandData> c3(ctEepromBasicPidsAdditional);
    memcpy((char *)(&c3.data()) + 1, &Memory::getInstance().data().basicPidsAdditionalData, sizeof(c3.data())-1 );
    Serial.write((quint8 *)c3.msg(), c3.serializedSize());
    Serial.flush();
}

void(* resetFunc) (void) = 0;
/*void resetFunc(){
    wdt_disable(); 
    wdt_enable(WDTO_15MS);
    while (1) {}    
}*/
void reset_Reaction_r(Base_CommandData * data){    
    resetFunc();
}

void clearEeprom_Reaction_r(Base_CommandData * data){
    //ClearEeprom_CommandData * d = (ClearEeprom_CommandData *)(data);
    Memory::getInstance().clear();
    //Command<ClearEeprom_CommandData> c(ctClearEeprom);
    //c.data().error = 0;
    //Serial.write((quint8 *)c.msg(), c.serializedSize());
}


void eeprom1_Reaction_r(Base_CommandData * data){
    Command<Eeprom1_CommandData> c(ctEeprom1);
    c.data().firstTime = Memory::getInstance().data().firstTime;
    c.data().version = Memory::getInstance().data().version;
    Serial.write((quint8 *)c.msg(), c.serializedSize());
    Command<EepromReceiver_CommandData> c1(ctEepromReceiver);
    memcpy((char *)(&c1.data()) + 1, &Memory::getInstance().data().rc, sizeof(c1.data())-1 );    
    Serial.write((quint8 *)c1.msg(), c1.serializedSize());
    Serial.flush();
    outPids();
    //memcpy((char *)(&c.data()) + 1, &Memory::getInstance().data(), sizeof(c.data())-1 );    
    
}

void calibrateRadio_Reaction_r(Base_CommandData * data){
    receiverRef.calibrate();
}

void setPid_Reaction_r(Base_CommandData * data){
    SetPid_CommandData * setPidData = (SetPid_CommandData *)(data);
    qint16 * pointer = (qint16 *) &(Memory::getInstance().data().basicPidsData);

    pointer += setPidData->offset;
    *pointer = setPidData->value;
    //memcpy(pointer + setPidData->offset, &setPidData->value, 2); 
    Memory::getInstance().save();

    outPids();
}

void onRadioCalibrationFinished(){
    Command<CalibrateRadio_CommandData> c(ctCalibrateRadio);
    c.data().error = 0;
    memcpy((char *)(&c.data()) + 1, &Memory::getInstance().data(), sizeof(c.data())-1 );
    Serial.write((quint8 *)c.msg(), c.serializedSize());    
}

int freeRam () {
    extern int __heap_start, *__brkval;
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

quint32 now, lastNow;
quint16 fullIterationPeriod;
quint16 iterationPeriod;

void setup(){    
    pinMode(ARM_LED_PIN, OUTPUT);
    pinMode(WORK_LED_PIN, OUTPUT);

    Serial.begin(115200);
    //Serial.println("setup()");
    
  
  //char pins[] = {A0, A1, A2, A3};
  //initEfRc(pins, 4);

    //Serial.println("receiverRef.init();");
    receiverRef.init();
    receiverRef.onCalibrationFinished = onRadioCalibrationFinished;
    //Serial.println("motorsRef.init();");
    motorsRef.init(A0,A1,A3,A2);

    //Serial.println("sensorRef.init();");
    sensorRef.init();


    lastNow = micros();
    fullIterationPeriod =0;
    iterationPeriod = 0;
  
    reset_Reaction = reset_Reaction_r;
    clearEeprom_Reaction = clearEeprom_Reaction_r;
    eeprom1_Reaction = eeprom1_Reaction_r;

    calibrateRadio_Reaction = calibrateRadio_Reaction_r;

    setPid_Reaction = setPid_Reaction_r;

    //Serial.println("END setup");
}

void serialEvent(){
    char ch;
    while (Serial.available() > 0){
        ch = Serial.read();
        commandManagerRef.pushData(&ch, 1);
    }
}

quint8 fifoErrorCount = 0;

void workLed(){
    static quint32 lastTurn = 0;
    static quint32 period = 500;

    if (receiverRef.armed()){
        period = 100;
    }else{
        period = 500;
    }
    static bool light = true;
    quint32 now = millis();
    if (now - lastTurn > period){
        lastTurn = now;
        light = !light;
        if (light){
            digitalWrite(WORK_LED_PIN, HIGH);
        }else{
            digitalWrite(WORK_LED_PIN, LOW);
        }
    }

}
//========================================================================================
//                             MAIN LOOP
//========================================================================================
void loop(){
    quint32 now = micros();
    fullIterationPeriod = now - lastNow;    
    
    lastNow = now;

    while (!sensorRef.isReady()){
        if (micros() > now + 10000){
            sensorRef.resetFifo();
            fifoErrorCount ++;
            break;
         }
        //w8 for MPU data
    }
    sensorRef.update();
    sensorRef.mpuInterruptRef()= false; //todelete
  
    receiverRef.update();
    if (receiverRef.armed()){
        digitalWrite(ARM_LED_PIN, HIGH);
    }else{
        digitalWrite(ARM_LED_PIN, LOW);
    }

    
    
    motorsRef.setReceiverValues(
        receiverRef.tr(),
        receiverRef.y(),
        receiverRef.p(),
        receiverRef.r()
        );

    motorsRef.setSensorValues(
        sensorRef.ypr()[0]* 180.0 / M_PI,
        sensorRef.ypr()[1]* 180.0 / M_PI,
        sensorRef.ypr()[2]* 180.0 / M_PI,
        sensorRef.gravity().x,
        sensorRef.gravity().y,
        sensorRef.gravity().z
        );
    
    motorsRef.setMotorsWorking(receiverRef.armed() && !receiverRef.fullFailSafe());
    motorsRef.update();


    
    //=============================================================    
    Command<DebugInfo_CommandData> c(ctDebugInfo);      
    c.data().workTime = millis();
    c.data().iterationPeriod = iterationPeriod/1000;
    c.data().fullIterationPeriod = fullIterationPeriod/1000;
    c.data().armed = receiverRef.armed();
    c.data().fullFailSafe = receiverRef.fullFailSafe();
    c.data().freeRam = freeRam();
    c.data().fifoErrorCount = fifoErrorCount;
    c.data().serialErrorCount = commandManagerRef.errorCount();
    c.data().fifoOverflowCount = sensorRef.fifoOverflowCount();
    Serial.write((uint8_t*)(c.msg()), c.serializedSize());        



    //=============================================================
    Command<Rc_CommandData> c2(ctRc);
  
    c2.data().type = 4;
    c2.data().pure.tr = receiverRef.pure_tr();
    c2.data().pure.y = receiverRef.pure_y();
    c2.data().pure.p = receiverRef.pure_p();
    c2.data().pure.r = receiverRef.pure_r();
    //c2.data().pure.a1 = receiverRef.pure_a1();
    //c2.data().pure.a2 = receiverRef.pure_a2();
  
    c2.data().processed.tr = receiverRef.tr();
    c2.data().processed.y = receiverRef.y();
    c2.data().processed.p = receiverRef.p();
    c2.data().processed.r = receiverRef.r();
    //c2.data().processed.a1 = receiverRef.a1();
    //c2.data().processed.a2 = receiverRef.a2();

    Serial.write((uint8_t*)(c2.msg()), c2.serializedSize());
    
  //---------------------------------------------------------------
    
    Command<Mpu_CommandData> c3(ctMpu);   
  
    c3.data().y= sensorRef.ypr()[0] * 180.0 / M_PI;
    c3.data().p = sensorRef.ypr()[1] * 180.0 / M_PI;
    c3.data().r = sensorRef.ypr()[2] * 180.0 / M_PI;
    c3.data().gx = sensorRef.gravity().x;
    c3.data().gy = sensorRef.gravity().y;
    c3.data().gz = sensorRef.gravity().z;
    
    Serial.write((uint8_t*)(c3.msg()), c3.serializedSize()); 
    
  //---------------------------------------------------------------
    
    Command<Motor_CommandData> c4(ctMotor);   
  
    c4.data().fl = motorsRef.motorValues().fl;
    c4.data().fr = motorsRef.motorValues().fr;
    c4.data().bl = motorsRef.motorValues().bl;
    c4.data().br = motorsRef.motorValues().br;
    
    Serial.write((uint8_t*)(c4.msg()), c4.serializedSize());

    
  //---------------------------------------------------------------  
    workLed();
    
    //Serial.flush();
    
    commandManagerRef.proceedDataBuffer();
    commandManagerRef.pop();
    
    //delay(30);
    
    iterationPeriod = micros() - now;
    
    if (FULL_ITERATION_PERIOD * 1000 > iterationPeriod){
        delayMicroseconds(FULL_ITERATION_PERIOD * 1000 - iterationPeriod);
    }
    
}

