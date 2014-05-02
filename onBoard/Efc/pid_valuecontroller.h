#ifndef PID_VALUECONTROLLER_H
#define PID_VALUECONTROLLER_H
//#include "p_valuecontroller.h"

class PID_ValueController
{
public:
    PID_ValueController();


    float P() const;
    void setP(float P);

    float I() const;
    void setI(float I);

    float D() const;
    void setD(float D);

    void setMaxI(float maxI);

    void setGlobalMultiplier(float value);

    virtual float getF(float error, float dt);

protected:
    float _P;
    float _I;
    float _D;

    float _maxI;

    float _globalMultiplier;

    float _integral;
    float _lastError;

};

#endif // PID_VALUECONTROLLER_H
