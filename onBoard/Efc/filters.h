#ifndef FILTERS_H
#define FILTERS_H
#include "defines.h"


class AverageFilter{
public:
    AverageFilter();

    void setK(float k){
        _k = k;
    };

    float add(float newX);

    float get(){
        return _x;
    }
protected:
    float _k;
    float _x;

};


/*class EFilter
{
public:
    EFilter();

    //добавить точку (время и значение)
    float add(quint32 time, float x);

    //получить прогноз значения на время time
    float get(quint32 time);

    //выставить коэффициент доверия измерениям
    void setK(float k);
    //выставить минимальное число точек для стабильных измерений
    void setStableN(quint32 stableN);
protected:
    float add1(quint32 time, float x);
    float add2(quint32 time, float x);
    float normalAdd(quint32 time, float x);

    quint32 _lastAddedT;

    float _lastX;
    float _v;
    int nDots;

    float _k;

    int _stableN;

    float currentK();

};
*/



#endif