#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
//#include "MPU6050.h" // not necessary if using MotionApps include file
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    //#include "../libraries/Wire/Wire.h"
#include "Wire.h"
#endif



struct VectorFloat;
/*
class ISensor
{
public:

    static ISensor& getInstance();
    
    virtual float* ypr() = 0;

    virtual VectorFloat & gravity() = 0;
  
    virtual bool init() = 0;
  
    virtual bool update() = 0;

    virtual bool isReady() = 0;

    virtual volatile bool& mpuInterruptRef() = 0;

    virtual void resetFifo() = 0;

    virtual quint8 fifoOverflowCount();
};
*/





class Sensor
{
public:
    static Sensor& getInstance(){
        static Sensor instance;
        return instance;
    }

    float * ypr(){
        return _filteredYpr;
    }

    VectorFloat & gravity(){
        return _filteredGravity;
        //return _gravity;
    }
  
    bool init();
  
    bool update();

    bool isReady();

    volatile bool& mpuInterruptRef()
    {
        return mpuInterrupt;
    }

    void resetFifo();

    quint8 fifoOverflowCount(){
        return _fifoOverflowCount;
    }
  
protected:
    quint8 _fifoOverflowCount;
    volatile bool mpuInterrupt;

    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO

    //AverageFilter _fY, _fP, _fR, _fGx, _fGy, _fGz;
    MPU6050 mpu;

    // MPU control/status vars
    bool dmpReady;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint8_t fifoBuffer[64]; // FIFO storage buffer
    
    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    //VectorFloat _gravity;    // [x, y, z]            gravity vector
    //float _ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    float _filteredYpr[3];
    VectorFloat _filteredGravity;
  
};



//===================================================================================================================
//===================================================================================================================
//===================================================================================================================

void dmpDataReady(){
    Sensor::getInstance().mpuInterruptRef() = true;
}


bool Sensor::isReady()
{
    return mpuInterrupt || fifoCount >= packetSize;
}


void Sensor::resetFifo(){
    mpu.resetFIFO();
}

bool Sensor::update(){


//    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();


    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    mpuInterrupt = false;

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        _fifoOverflowCount ++;
        mpu.resetFIFO();
        //fifoCount = 0;
//        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else
    if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        while (fifoCount >= packetSize){
            mpu.getFIFOBytes(fifoBuffer, packetSize);
             
             // track FIFO count here in case there is > 1 packet available
             // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
        }
        
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&_filteredGravity, &q);
            mpu.dmpGetYawPitchRoll(_filteredYpr, &q, &_filteredGravity);

            //Serial.print("ypr\t");
            //Serial.print(_filteredYpr[0] * 180/M_PI);
            //Serial.print("\t");
            //Serial.print(_filteredYpr[1] * 180/M_PI);
            //Serial.print("\t");
            //Serial.println(_filteredYpr[2] * 180/M_PI);            

            //_filteredYpr[0] = _fY.add(_filteredYpr[0]);
            //_filteredYpr[1] = _fP.add(_filteredYpr[1]);
            //_filteredYpr[2] = _fR.add(_filteredYpr[2]);

            //_filteredGravity.x = _fGx.add(_filteredGravity.x);
            //_filteredGravity.y = _fGy.add(_filteredGravity.y);
            //_filteredGravity.z = _fGz.add(_filteredGravity.z);


        // blink LED to indicate activity
    }
}



//===================================================================================================================
//===================================bool Sensor::init()=============================================================
//===================================================================================================================


bool Sensor::init(){
/*    mpuInterrupt = false;
    _filteredYpr[0] = 0;
    _filteredYpr[1] = 0;
    _filteredYpr[2] = 0;
    _filteredGravity.x = 0;
    _filteredGravity.y = 0;
    _filteredGravity.z = 0;
*/
    _fifoOverflowCount = 0;



    //_fY.setK(1.0);
    //_fP.setK(1.0);
    //_fR.setK(1.0);
    //_fGx.setK(1.0);
    //_fGy.setK(1.0);
    //_fGz.setK(1.0);




    dmpReady = false;


    
    // join I2C bus (I2Cdev library doesn't do this automatically)
//    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
//    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//        Fastwire::setup(400, true);
//    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    mpu.initialize();

    // verify connection
    mpu.testConnection(); //bool

    // wait for ready

    // load and configure the DMP
    devStatus = mpu.dmpInitialize();

// x accel: -3641
// y accel: -3603
// z accel: 1589
// x gyro: 105
// y gyro: 5
// z gyro: -21

    mpu.setXGyroOffset(105);
    mpu.setYGyroOffset(5);
    mpu.setZGyroOffset(-21);
    
    mpu.setXAccelOffset(-3641);
    mpu.setYAccelOffset(-3603);
    mpu.setZAccelOffset(1589);

    //mpu.setRate(0);


    // make sure it worked (returns 0 if so)
    //if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

        mpu.setDLPFMode(1);

    //} else {
        // ERROR! (devStatus code:)
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
    //}
    
}





	

#endif

