#include "dsm501.h"



/*
    20130527    添加乐联老刘的滤波算法(QQ 521419)
    20130528    重载了getParticles方法，这样可以对比滤波与未滤波之间的差别
*/

dsm501::dsm501()
{
    filterEnabled=false ;
}

dsm501::~dsm501()
{
}

void dsm501::setDatapin(int pin)
{
    pinData = pin;//指定arduino上数据输出脚
    pinMode(pinData,INPUT);
}

int dsm501::getParticles()
{
    int fv;
    int ov;
    getParticles(ov,fv);

    return fv; //如果启用了滤波，返回滤波后的结果，否则fv=ov
}


void dsm501::enableFilter()
{
    filterEnabled=true ;
}

void dsm501::disableFilter()
{
    filterEnabled=false ;
}

unsigned long dsm501::Array_Average( unsigned long* Array,int length)
{
    int x;
    unsigned long returnVal;
    unsigned long result=0;
    for(x=0; x<length; x++)
    {
        result+=Array[x];
        //Serial.print("result=");
        //Serial.println(result);
    }
    returnVal=result/length;
    return returnVal;
}

unsigned long dsm501::Filter1(unsigned long lowpulse)
{
    static unsigned long sfiterArray[filterlen];
    static int sindex=0;
    int x;

#ifdef DEBUGGING
    Serial.println("filter1 begin:");
#endif

    if(filterlen>sindex)
    {
        sindex++;


#ifdef DEBUGGING
        Serial.println(sindex);
#endif

        sfiterArray[sindex]=lowpulse;

#ifdef DEBUGGING
        Serial.println("filter1 END");
#endif

        return lowpulse;
    }
    else
    {
        for(x=0; x<filterlen-1; x++)
        {
            sfiterArray[x]=sfiterArray[x+1];
        }
        sfiterArray[filterlen-1]=lowpulse;


#ifdef DEBUGGING
        for(x=0; x<filterlen; x++)
        {
            Serial.println(sfiterArray[x]);
        }
        Serial.println("Aver:");
        Serial.println(Array_Average(sfiterArray,filterlen));
        Serial.println("filter1 END");
#endif

        return(Array_Average(sfiterArray,filterlen));

    }
}

int calculateConcentration(int value)
{
    float r=0.0;

    r=value / (sampleTime_ms*10.0);
    return ( 1.1*pow(r,3)-3.8*pow(r,2)+520*r+0.62);

}

void dsm501::getParticles(int &originValue, int &filteredValue)
{
    //主要是用来对比滤波效果
    int concentration=0;
    float ratioPWM=0.0 ;
    unsigned long lowPulseOccupancy=0;
    unsigned long startTime=0;
    unsigned long duration=0;

    startTime= millis();

    while ((millis()-startTime)<= sampleTime_ms)//独占30秒
    {
        duration =pulseIn(pinData, LOW);   //读低电平时间
        lowPulseOccupancy += duration;

#ifdef DEBUGGING
//下面代码会影响传感器的返回值，因此工作状态要注释掉
        int t=millis();
        Serial.print("D/L");
        Serial.print("\t");
        Serial.print(duration);
        Serial.print("\t");
        Serial.println(lowPulseOccupancy);
        Serial.println(" ");
        Serial.print("milliseconds used: ");
        Serial.println(millis()-t);
        Serial.println(" ");
#endif
    }

    //计算颗粒物浓度
    //如果采样时间长于规定的长度--30s
    originValue=calculateConcentration(lowPulseOccupancy);

    if (filterEnabled)
    {
        lowPulseOccupancy=Filter1(lowPulseOccupancy);
        filteredValue=calculateConcentration(lowPulseOccupancy);
    }
    else
    {
        filteredValue=originValue;
    }

#ifdef DEBUGGING
    Serial.print(">>>>>>>>>>R/C");
    Serial.print("\t");
    Serial.print(ratioPWM);
    Serial.print("\t");
    Serial.println(concentration);
    Serial.println(" ");
#endif

}
