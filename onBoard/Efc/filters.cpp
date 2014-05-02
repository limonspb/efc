#include "filters.h"



float AverageFilter::add(float newX){
    _x = _x + _k * (newX - _x);
    return _x;
}

AverageFilter::AverageFilter()
    : _k(0.1)
    , _x(0.0)
{}





/*EFilter::EFilter()
{
    _lastAddedT = 0;
    nDots = 0;
    _k = 0.1;
    _stableN = 10;
}

float EFilter::add(quint32 time, float x)
{
    float res = 0;
    if (nDots == 0){
        res = add1(time, x);
    }else if (nDots == 1){
        res = add2(time, x);
    }else{
        res = normalAdd(time, x);
    }
    return res;
}

float EFilter::get(quint32 time)
{
    if (nDots > 1){
        qint32 dt = time - _lastAddedT;
        return _lastX + dt * _v;
    }else{
        return _lastX;
    }

}

void EFilter::setK(float k)
{
    _k = k;
}

void EFilter::setStableN(quint32 stableN)
{
    _stableN = stableN;
}

float EFilter::add1(quint32 time, float x)
{
    this->_lastAddedT = time;
    this->_lastX = x;
    nDots ++;
    return x;
}

float EFilter::add2(quint32 time, float x)
{
    qint32 dt = time - _lastAddedT;
    float dx = x - _lastX;
    _lastX = x;
    _lastAddedT = time;
    if (dt != 0){
        nDots++;
        _v = dx/float(dt);
    }else{

    }

    return x;
}

float EFilter::normalAdd(quint32 time, float x)
{
    qint32 dt = time - _lastAddedT;

    float countedX = 0;

    countedX = _lastX + _v * dt;

    float error = x - countedX;

    float newX = countedX + error * currentK();

    float dx = newX - _lastX;

    _lastX = newX;

    _lastAddedT = time;

    if (dt != 0){
        _v = dx/float(dt);
    }else{

    }

    nDots++;

    return newX;

}

float EFilter::currentK()
{
    if (nDots >= _stableN){
        return _k;
    }else{
        return _k + float(_stableN - nDots)/float(_stableN) * (1.0 - _k);
    }
}*/