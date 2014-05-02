//#include "sensor.h"
//#include <Arduino.h>
//#include "filters.h"
//#include "defines.h"

//#include <Arduino.h>





/*

bool Sensor::getYpr(float & y, float & p, float r){
    _mpuIntStatus = _mpu.getIntStatus();

    // get current FIFO count
    _fifoCount = _mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((_mpuIntStatus & 0x10) || _fifoCount == 1024) {
        // reset so we can continue cleanly
        _mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));
        
        return false;

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (_mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (_fifoCount < _packetSize) _fifoCount = _mpu.getFIFOCount();

        // read a packet from FIFO
        _mpu.getFIFOBytes(_fifoBuffer, _packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        _fifoCount -= _packetSize;

        #ifdef OUTPUT_READABLE_QUATERNION
            // display quaternion values in easy matrix form: w x y z
            _mpu.dmpGetQuaternion(&_q, _fifoBuffer);
            Serial.print("quat\t");
            Serial.print(_q.w);
            Serial.print("\t");
            Serial.print(_q.x);
            Serial.print("\t");
            Serial.print(_q.y);
            Serial.print("\t");
            Serial.println(_q.z);
        #endif

        #ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
            _mpu.dmpGetQuaternion(&_q, _fifoBuffer);
            _mpu.dmpGetEuler(_euler, &_q);
            Serial.print("euler\t");
            Serial.print(_euler[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(_euler[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(_euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            _mpu.dmpGetQuaternion(&_q, _fifoBuffer);
            _mpu.dmpGetGravity(&_gravity, &_q);
            _mpu.dmpGetYawPitchRoll(_ypr, &_q, &_gravity);
            Serial.print("ypr\t");
            Serial.print(_ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(_ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(_ypr[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            _mpu.dmpGetQuaternion(&_q, _fifoBuffer);
            _mpu.dmpGetAccel(&_aa, _fifoBuffer);
            _mpu.dmpGetGravity(&_gravity, &_q);
            _mpu.dmpGetLinearAccel(&_aaReal, &_aa, &_gravity);
            Serial.print("areal\t");
            Serial.print(_aaReal.x);
            Serial.print("\t");
            Serial.print(_aaReal.y);
            Serial.print("\t");
            Serial.println(_aaReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            _mpu.dmpGetQuaternion(&_q, _fifoBuffer);
            _mpu.dmpGetAccel(&_aa, _fifoBuffer);
            _mpu.dmpGetGravity(&_gravity, &_q);
            _mpu.dmpGetLinearAccel(&_aaReal, &_aa, &_gravity);
            _mpu.dmpGetLinearAccelInWorld(&_aaWorld, &_aaReal, &_q);
            Serial.print("aworld\t");
            Serial.print(_aaWorld.x);
            Serial.print("\t");
            Serial.print(_aaWorld.y);
            Serial.print("\t");
            Serial.println(_aaWorld.z);
        #endif    
    }  
}



bool Sensor::init(){
  bool dmpReady = false;  // set true if DMP init was successful
  
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    _mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(_mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    _devStatus = _mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    _mpu.setXGyroOffset(220);
    _mpu.setYGyroOffset(76);
    _mpu.setZGyroOffset(-85);
    _mpu.setZAccelOffset(1688); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (_devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        _mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
//        attachInterrupt(0, dmpDataReady, RISING);
        _mpuIntStatus = _mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        _packetSize = _mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(_devStatus);
        Serial.println(F(")"));
    }  
    return dmpReady;
}
*/
