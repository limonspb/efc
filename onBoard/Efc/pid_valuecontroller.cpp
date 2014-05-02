#include "pid_valuecontroller.h"

PID_ValueController::PID_ValueController()
/*
    : _P(1.0)
    , _I(1.0)
    , _D(1.0)
    , _integral(0.0)
    , _maxI(0.0)
    , _globalMultiplier(1.0)
    , _lastError(0.0)
    */

{
}

float PID_ValueController::I() const
{
    return _I;
}


void PID_ValueController::setI(float I)
{
    _I = I;
}

float PID_ValueController::D() const
{
    return _D;
}

void PID_ValueController::setD(float D)
{
    _D = D;
}

float PID_ValueController::P() const
{
    return _P;
}

void PID_ValueController::setP(float P)
{
    _P = P;
}

void PID_ValueController::setMaxI(float maxI){
    _maxI = maxI;
}

void PID_ValueController::setGlobalMultiplier(float value){
    _globalMultiplier = value;
}

float PID_ValueController::getF(float error, float dt)
{
    //dt = 0.1;
    _integral += error * _I * dt;    

    if (_maxI > 0){
        if (_integral >  _maxI) _integral =   _maxI;
        if (_integral < -_maxI) _integral = - _maxI;
    }


    float P = _P * error;
    float I = _integral;

    float D = _D*(error - _lastError)/dt;
    _lastError = error;


    return (P + I + D) * _globalMultiplier;


}
