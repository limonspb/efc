#ifndef RECEIVER_H
#define RECEIVER_H

#define CALIBRATION_TIME 10000 //millis
#define RC_SAFE_THROTTLE_OFFSET 60 //minimum throttle to start motors
#define AUTO_DISARM_TIME 2000
#define HOLD_TIME_TO_ARM_OR_DISARM 1500

#include "EfRc.h"
#include "defines.h"
#include "memory.h"

#include "filters.h"
class Receiver{
public:
    static Receiver & getInstance(){
        static Receiver instance;
        return instance;
    }  
  
//    void (* onArm)();
//    void (* onDisarm)();
    void (* onCalibrationFinished)();

    void init();

    void update();

    bool isCalibrated();
    bool calibrate();
    bool fullFailSafe(){return _fullFailSaife;}
    bool armed(){return _armed;}

    const quint16 & pure_tr(){ return _pure_tr; }
    const quint16 & pure_y(){ return _pure_y; }
    const quint16 & pure_p(){ return _pure_p; }
    const quint16 & pure_r(){ return _pure_r; }
    //const quint16 & pure_a1(){ return _pure_a1; }
    //const quint16 & pure_a2(){ return _pure_a2; }

    const quint16 & tr(){ return _tr; }
    const quint16 & y(){ return _y; }
    const quint16 & p(){ return _p; }
    const quint16 & r(){ return _r; }
    //const quint16 & a1(){ return _a1; }
    //const quint16 & a2(){ return _a2; }

protected:
    quint16 _pure_tr;
    quint16 _pure_y;
    quint16 _pure_p;
    quint16 _pure_r;

    AverageFilter _filter_tr, _filter_r, _filter_y, _filter_p;
    //quint16 _pure_a1;
    //quint16 _pure_a2;

    quint16 _tr;
    quint16 _y;
    quint16 _p;
    quint16 _r;
    //quint16 _a1;
    //quint16 _a2;

    bool _fullFailSaife;
    bool _armed;

    void checkForArming();
    quint32 _last_not_BR;
    quint32 _last_not_BL;
    quint32 _last_tr;

    Receiver();
    Receiver(const Receiver &){}
    Receiver& operator=( Receiver& ){}

    quint32 _calibrationStartTime;
    bool _calibration;

    void calibration();
    void stopCalibration();

    quint32 _armedTime;
};

#endif
