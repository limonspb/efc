#ifndef MOTORS_H
#define MOTORS_H

#include "defines.h"
#include <Servo.h>
#include "pid_valuecontroller.h"

class Motors{
public:
    static Motors & getInstance(){
        static Motors instance;
        return instance;
    }

    void init(quint8 pin_fl, quint8 pin_fr, quint8 pin_bl, quint8 pin_br);
    void readPidsFromMem();

    void setReceiverValues(qint16 tr, qint16 y, qint16 p, qint16 r);

    void setMotorsWorking(bool motorsWorking){
        if (_motorsWorking != motorsWorking){
            if (motorsWorking){
                doArm();
            }else{
                doDisarm();
            }
        }
        _motorsWorking = motorsWorking;
    }

    void doArm();

    void doDisarm();

    void setSensorValues(float y, float p, float r, float gX, float gY, float gZ);

    struct MotorValues{
        float fl;
        float fr;
        float bl;
        float br;
    };

    MotorValues & motorValues();

    void update();

protected:
    Servo _fl, _fr, _bl, _br;
    PID_ValueController _pid_y, _pid_p, _pid_r;

    MotorValues _motorValues;

    float _sensor_y;
    float _sensor_p;
    float _sensor_r;
    float _sensor_gX;
    float _sensor_gY;
    float _sensor_gZ;

    float _rc_tr;
    float _rc_y;
    float _rc_p;
    float _rc_r;

    quint8 _pin_fl;
    quint8 _pin_fr;
    quint8 _pin_bl;
    quint8 _pin_br;

    bool _motorsWorking;

    float angleDiff(float a1, float a2);


    Motors(){}
    Motors(const Motors &);
    Motors & operator=(const Motors &);

    float _lastTr;
    float _targetYaw;
};

#endif