#include "motors.h"

#include "memory.h"

#include "configs.h"

#include <Arduino.h>

void Motors::setSensorValues(float y, float p, float r, float gX, float gY, float gZ){
    _sensor_y = y;
    _sensor_p = p;
    _sensor_r = r;
    _sensor_gX = gX;
    _sensor_gY = gY;
    _sensor_gZ = gZ;
}

void Motors::setReceiverValues(qint16 tr, qint16 y, qint16 p, qint16 r){
    _rc_tr = float(tr-1000)/1000.0;
    _rc_y  = float(y-1500)/500.0;
    _rc_p  = float(p-1500)/500.0;
    _rc_r  = float(r-1500)/500.0;
}

float int_to_float(qint16 i){
    return float(i)/1000.0;
}

void Motors::update(){
    if (_lastTr == 0 && _rc_tr > 0){
        _targetYaw = _sensor_y;
    }
    _lastTr = _rc_tr;

    float temp1, temp2, temp3 = 0;

    if (!_motorsWorking || _rc_tr == 0){
        _motorValues.fl = 0.0;
        _motorValues.fr = 0.0;
        _motorValues.bl = 0.0;
        _motorValues.br = 0.0;
    }else{
        /*
        float middleValue = _rc_tr;
        float maxChanging = 0.3 * _rc_tr;

        float fl, fr, bl, br;

        fl = middleValue + maxChanging * (   _rc_y  -  _rc_p  +  _rc_r);
        fr = middleValue + maxChanging * (-  _rc_y  -  _rc_p  -  _rc_r);
        bl = middleValue + maxChanging * (-  _rc_y  +  _rc_p  +  _rc_r);
        br = middleValue + maxChanging * (   _rc_y  +  _rc_p  -  _rc_r);

        _motorValues.fl = fl;
        _motorValues.fr = fr;
        _motorValues.bl = bl;
        _motorValues.br = br;
        */

        float dt = int_to_float(FULL_ITERATION_PERIOD);

        static quint32 last = 0;
        quint32 now = micros();
        dt = float(now-last)/1000000.0;
        last = now;

        
        _targetYaw = _targetYaw + _rc_y * MAX_YAW_SPEED * (dt);
        float ta_y =   _targetYaw;
        float ta_p = - _rc_p * MAX_P_DECLINATION;        
        float ta_r =   _rc_r * MAX_R_DECLINATION;



        float f_y = _pid_y.getF(angleDiff(ta_y, _sensor_y) , dt);
        float f_p = _pid_p.getF(ta_p -_sensor_p, dt);
        float f_r = _pid_r.getF(ta_r - _sensor_r, dt);
        //f_y = 0;

        temp1 = ta_y;
        temp2 = _sensor_y;
        temp3 = f_r;


        f_y = constrain(f_y, -MAX_PID_FORCE, MAX_PID_FORCE)/MAX_PID_FORCE;
        f_p = constrain(f_p, -MAX_PID_FORCE, MAX_PID_FORCE)/MAX_PID_FORCE;
        f_r = constrain(f_r, -MAX_PID_FORCE, MAX_PID_FORCE)/MAX_PID_FORCE;

        
        
        //float maxChanging = 1.4 * _rc_tr;
        //maxChanging = 1.0;

        //_motorValues.fl = _rc_tr + maxChanging * (-  f_y  +  f_p  +  f_r);
        //_motorValues.fr = _rc_tr + maxChanging * (   f_y  +  f_p  -  f_r);
        //_motorValues.bl = _rc_tr + maxChanging * (   f_y  -  f_p  +  f_r);
        //_motorValues.br = _rc_tr + maxChanging * (-  f_y  -  f_p  -  f_r);

        _motorValues.fl = _rc_tr + (-  f_y  +  f_p  +  f_r);
        _motorValues.fr = _rc_tr + (   f_y  +  f_p  -  f_r);
        _motorValues.bl = _rc_tr + (   f_y  -  f_p  +  f_r);
        _motorValues.br = _rc_tr + (-  f_y  -  f_p  -  f_r);

        float maxMotorValue = _motorValues.fl;
        maxMotorValue = max(maxMotorValue, _motorValues.fr);
        maxMotorValue = max(maxMotorValue, _motorValues.bl);
        maxMotorValue = max(maxMotorValue, _motorValues.br);

        if (maxMotorValue > 1.0){
            _motorValues.fl /= maxMotorValue;
            _motorValues.fr /= maxMotorValue;
            _motorValues.bl /= maxMotorValue;
            _motorValues.br /= maxMotorValue;
        }

        if (_motorValues.fl < 0) _motorValues.fl = 0;
        if (_motorValues.fr < 0) _motorValues.fr = 0;
        if (_motorValues.bl < 0) _motorValues.bl = 0;
        if (_motorValues.br < 0) _motorValues.br = 0;

        

    }

    _fl.writeMicroseconds(_motorValues.fl * (MOTOR_MAX - MOTOR_MIN) + MOTOR_MIN);
    _fr.writeMicroseconds(_motorValues.fr * (MOTOR_MAX - MOTOR_MIN) + MOTOR_MIN);
    _bl.writeMicroseconds(_motorValues.bl * (MOTOR_MAX - MOTOR_MIN) + MOTOR_MIN);
    _br.writeMicroseconds(_motorValues.br * (MOTOR_MAX - MOTOR_MIN) + MOTOR_MIN);

    //_motorValues.bl = temp1; //HERE!!!
    //_motorValues.br = temp2; //HERE!!!
    //_motorValues.fl = temp3;


}

void Motors::doArm(){
    _fl.attach(_pin_fl);
    _fr.attach(_pin_fr);
    _bl.attach(_pin_bl);
    _br.attach(_pin_br);
    _fl.writeMicroseconds(1000);
    _fr.writeMicroseconds(1000);
    _bl.writeMicroseconds(1000);
    _br.writeMicroseconds(1000);
}
void Motors::doDisarm(){
    _fl.writeMicroseconds(1000);
    _fr.writeMicroseconds(1000);
    _bl.writeMicroseconds(1000);
    _br.writeMicroseconds(1000);
    _fl.detach();
    _fr.detach();
    _bl.detach();
    _br.detach();

}


void Motors::init(quint8 pin_fl, quint8 pin_fr, quint8 pin_bl, quint8 pin_br){
    _pin_bl = pin_bl;
    _pin_fl = pin_fl;
    _pin_fr = pin_fr;
    _pin_br = pin_br;
    doArm();
    doDisarm();


    _lastTr = 0;
    _targetYaw = 0;

    _motorsWorking = false;

    readPidsFromMem();
}

void Motors::readPidsFromMem(){
    Memory & memoryRef = Memory::getInstance();
    _pid_y.setP(INT_TO_FLOAT(memoryRef.data().basicPidsData.y.P));
    _pid_y.setI(INT_TO_FLOAT(memoryRef.data().basicPidsData.y.I));
    _pid_y.setD(INT_TO_FLOAT(memoryRef.data().basicPidsData.y.D));
    _pid_p.setP(INT_TO_FLOAT(memoryRef.data().basicPidsData.p.P));
    _pid_p.setI(INT_TO_FLOAT(memoryRef.data().basicPidsData.p.I));
    _pid_p.setD(INT_TO_FLOAT(memoryRef.data().basicPidsData.p.D));
    _pid_r.setP(INT_TO_FLOAT(memoryRef.data().basicPidsData.r.P));
    _pid_r.setI(INT_TO_FLOAT(memoryRef.data().basicPidsData.r.I));
    _pid_r.setD(INT_TO_FLOAT(memoryRef.data().basicPidsData.r.D));

    _pid_y.setMaxI(memoryRef.data().basicPidsAdditionalData.y.maxI * 100);
    _pid_p.setMaxI(memoryRef.data().basicPidsAdditionalData.p.maxI * 100);
    _pid_r.setMaxI(memoryRef.data().basicPidsAdditionalData.r.maxI * 100);

    _pid_y.setGlobalMultiplier(INT_TO_FLOAT(memoryRef.data().basicPidsAdditionalData.y.globalMultiplier));
    _pid_p.setGlobalMultiplier(INT_TO_FLOAT(memoryRef.data().basicPidsAdditionalData.p.globalMultiplier));
    _pid_r.setGlobalMultiplier(INT_TO_FLOAT(memoryRef.data().basicPidsAdditionalData.r.globalMultiplier));
    //_pid_y.setI(0);
    //_pid_p.setI(0);
    //_pid_r.setI(0);
}

Motors::MotorValues & Motors::motorValues(){
    return _motorValues;
}


float Motors::angleDiff(float a1, float a2){
    while (a1 >= 360.0){
        a1 -= 360.0;
    }
    while (a2 >= 360.0){
        a2 -= 360.0;
    }

    while (a1 < 0.0){
        a1 += 360.0;
    }
    while (a2 < 0.0){
        a2 += 360.0;
    }

    float d1, d2;
    d1 = a1 - a2;
    if (a1 > a2){
        d2 = - 360.0 + a1 - a2;
    }else{
        d2 =   360.0 - a2 + a1;
    }

    if (abs(d1) < abs(d2)){
        return d1;
    }else{
        return d2;
    }



}