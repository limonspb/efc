#include "receiver.h"

#include <Arduino.h>
#include <math.h>
#include "configs.h"

void Receiver::init(){
    char pins[] = {10, 11, 12, 13};
    initEfRc(pins, 4);
    _fullFailSaife = false;
    _armed = false;
    onCalibrationFinished = NULL;
    _last_not_BR = 0;
    _last_not_BL = 0;
    _last_tr = 0;
    _calibration = false;
    _armedTime = 0;

    //TODO a1, a2 channels
}

void Receiver::update(){
    _pure_tr = getPPMDelay(0);
    _pure_y = getPPMDelay(1);
    _pure_p = getPPMDelay(2);
    _pure_r = getPPMDelay(3);

    //_pure_a1 = 900; //TODO
    //_pure_a2 = 900; //TODO

    if (_armed){
        _tr = constrain(map(_filter_tr.add(_pure_tr), Memory::getInstance().data().rc.minTr + RC_SAFE_THROTTLE_OFFSET, Memory::getInstance().data().rc.maxTr, 1000, 2000), 1000, 2000);
        _y = constrain(map(_filter_y.add(_pure_y), Memory::getInstance().data().rc.minY, Memory::getInstance().data().rc.maxY, 1000, 2000), 1000, 2000);
        _p = constrain(map(_filter_p.add(_pure_p), Memory::getInstance().data().rc.minP, Memory::getInstance().data().rc.maxP, 1000, 2000), 1000, 2000);
        _r = constrain(map(_filter_r.add(_pure_r), Memory::getInstance().data().rc.minR, Memory::getInstance().data().rc.maxR, 1000, 2000), 1000, 2000);

        
        if (abs(qint16(_y) - qint16(1500) ) < RC_SAFE_YPR_MIDDLE ){
            _y = 1500;
        }
        /*
        if (abs(qint16(_p) - qint16(1500) ) < RC_SAFE_YPR_MIDDLE ){
            _p = 1500;
        }
        if (abs(qint16(_r) - qint16(1500) ) < RC_SAFE_YPR_MIDDLE ){
            _r = 1500;
        }
        */
        //_a1 = 1000; //TODO
        //_a2 = 1000; //TODO
    }else{
        _tr = 1000;
        _y = 1500;
        _p = 1500;
        _r = 1500;
        //_a1 = 1000;
        //_a2 = 1000;
    }

    if (_pure_tr < Memory::getInstance().data().rc.fsValue &&
        _pure_y < Memory::getInstance().data().rc.fsValue &&
        _pure_p < Memory::getInstance().data().rc.fsValue &&
        _pure_r < Memory::getInstance().data().rc.fsValue){
        this->_fullFailSaife = true;
    }else{
        this->_fullFailSaife = false;
    }
    calibration();
    if (!_calibration){
        checkForArming();
    }
    
}

bool Receiver::isCalibrated(){
    return
    Memory::getInstance().data().rc.maxP > 1500 && Memory::getInstance().data().rc.maxP < 2500 &&
    Memory::getInstance().data().rc.maxTr > 1500 && Memory::getInstance().data().rc.maxTr < 2500 &&
    Memory::getInstance().data().rc.maxY > 1500 && Memory::getInstance().data().rc.maxY < 2500 &&
    Memory::getInstance().data().rc.maxR > 1500 && Memory::getInstance().data().rc.maxR < 2500 &&

    Memory::getInstance().data().rc.minP < 1500 && Memory::getInstance().data().rc.minP > 990 &&
    Memory::getInstance().data().rc.minTr < 1500 && Memory::getInstance().data().rc.minTr > 990 &&
    Memory::getInstance().data().rc.minY < 1500 && Memory::getInstance().data().rc.minY > 990 &&
    Memory::getInstance().data().rc.minR < 1500 && Memory::getInstance().data().rc.minR > 990;
    
}

bool Receiver::calibrate(){
    this->_calibrationStartTime = millis();
    this->_calibration = true;
}

void Receiver::calibration(){
    if (this->_calibration){
        Memory::getInstance().data().rc.minTr = min(Memory::getInstance().data().rc.minTr, _pure_tr);        
        Memory::getInstance().data().rc.minY = min(Memory::getInstance().data().rc.minY, _pure_y);
        Memory::getInstance().data().rc.minP = min(Memory::getInstance().data().rc.minP, _pure_p);
        Memory::getInstance().data().rc.minR = min(Memory::getInstance().data().rc.minR, _pure_r);

        Memory::getInstance().data().rc.maxTr = max(Memory::getInstance().data().rc.maxTr, _pure_tr);
        Memory::getInstance().data().rc.maxY = max(Memory::getInstance().data().rc.maxY, _pure_y);
        Memory::getInstance().data().rc.maxP = max(Memory::getInstance().data().rc.maxP, _pure_p);
        Memory::getInstance().data().rc.maxR = max(Memory::getInstance().data().rc.maxR, _pure_r);

        if (millis() - this->_calibrationStartTime > CALIBRATION_TIME){
            this->stopCalibration();
        }
    }

}

void Receiver::stopCalibration(){
    this->_calibration = false;
    Memory::getInstance().save();
    if (this->onCalibrationFinished != NULL){
        this->onCalibrationFinished();
    }
}


void Receiver::checkForArming(){    
    quint32 now = millis();
    if ((qint16(_pure_tr) - qint16(Memory::getInstance().data().rc.minTr)) > qint16(RC_SAFE_THROTTLE_OFFSET)){
        _last_tr = now;    
    }    

    if (
        ( abs(qint16(_pure_tr) - qint16(Memory::getInstance().data().rc.minTr)) < RC_SAFE_THROTTLE_OFFSET) &&
        (abs(qint16(_pure_y) - qint16(Memory::getInstance().data().rc.minY)) < RC_SAFE_THROTTLE_OFFSET)
        )
    {
        //yaw - left, throttle - bottom
        //_last_tr == now;
    }else{
        _last_not_BL = now;
    }

    if (
        (abs(qint16(_pure_tr) - qint16(Memory::getInstance().data().rc.minTr)) < RC_SAFE_THROTTLE_OFFSET) &&
        (abs(qint16(Memory::getInstance().data().rc.maxY) - qint16(_pure_y)) < RC_SAFE_THROTTLE_OFFSET)
        )
    {
        //yaw - right, throttle - bottom
        //_last_tr == now;
    }else{
        _last_not_BR = now;
    }

    if( ((now - _last_tr) > AUTO_DISARM_TIME) && _armed && ((now - _armedTime) > 2*AUTO_DISARM_TIME) ) {
        _armed = false;
        //if (onDisarm != NULL) onDisarm();
    }
    if (((now - _last_not_BL) > HOLD_TIME_TO_ARM_OR_DISARM) && _armed){
        _armed = false;
        /*if (this->onDisarm != NULL){
            this->onDisarm();
        }*/
    }else
    if (((now - _last_not_BR) > HOLD_TIME_TO_ARM_OR_DISARM) && !_armed && !_fullFailSaife){
        if (this->isCalibrated() && !_fullFailSaife){
            _armed = true;
            _armedTime = now;
            //_last_tr == now;
            //_last_not_BR = now;
            //_last_not_BL = now;
            /*if (this->onArm != NULL){
                this->onArm();
            }*/
        }
    }

    if (_fullFailSaife && _armed){
        _armed = false;
        /*if (this->onDisarm != NULL){
            this->onDisarm();
        } */       
    }

}

Receiver::Receiver()
{
    _filter_tr.setK(0.05);
    _filter_y.setK(0.05);
    _filter_p.setK(0.05);
    _filter_r.setK(0.05);
}