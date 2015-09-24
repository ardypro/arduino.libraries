#ifndef DSM501_H
#define DSM501_H

#include "Arduino.h"

#define sampleTime_ms 30000
#define filterlen 10

//#define DEBUGGING

class dsm501
{
public:
    dsm501();
    virtual ~dsm501();
    void setDatapin(int pin) ;
    int getParticles() ;
    void getParticles(int &originValue, int &filteredValue); //可以对比滤波前后的数据变化
    void enableFilter();
    void disableFilter();
protected:
    unsigned long Filter1(unsigned long lowpulse);
    unsigned long Array_Average( unsigned long* Array,int length);
private:
    int pinData;
    int particles;
    bool filterEnabled;
};

#endif // DSM501_H
