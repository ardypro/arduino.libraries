#pragma once

#include "Arduino.h"

//TODO 
//根据arduino board类型设置长度
#define MAX_BOOL_LEN    2
#define MAX_INT_LEN		10
#define MAX_LONG_LEN	11
#define MAX_DOUBLE_LEN	20


typedef unsigned char byte;

inline byte lenOfInt(long value)
{
	byte ret = 1;
	if (value<0)
	{
		value = value * -1;
		ret++;
	}
	while (value>9)
	{
		value /= 10;
		ret++;
	}

	return ret;
}


inline byte lenOfDbl(float value, byte digits)
{
	byte ret = 1;
	if (value<0)
		ret++;
	ret += lenOfInt(abs((int)value)) + digits;
	return ret;
}

class conversion
{
public:
    conversion(void)
    {
    };

    ~conversion(void)
    {
    };

    byte boolToStr(const bool value, char* str)
    {
        if (value)
        {
            str[0]='1';
            str[1]='\0';
        }
        else
        {
            str[0]='0';
            str[1]='\0';
        }

        return 1;
    }

    byte intToStr(const int value, char* str)
    {
        //返回字符串长度
        unsigned int len;
        len=lenOfInt(value)+1;   //null终止符
        snprintf(str,len,"%d",value);

        return strlen(str);
    }

    byte uintToStr(const unsigned int value, char* str)
    {
        snprintf(str,12,"%u",value);
        return strlen(str);

    }

    byte longToStr(const long value, char* str)
    {
        snprintf(str,12,"%ld",value);
        return strlen(str);
    }

    byte ulongToStr(const unsigned long value, char* str)
    {
        snprintf(str,12,"%lu",value);
        return strlen(str);
    }

    byte doubleToStr(double value, char* str, byte digits=2)
    {
        if (isnan(value))
            return 0;

        if (isinf(value))
            return 0;

        if (value> 4294967040.0)
            return 0;

        if (value < -4294967040.0)
            return 0;

		dtostrf(value, 4, digits, str);

		return strlen(str);



        double rounding=0.5;
        for (byte i=0; i<digits; ++i)
            rounding /=10.0;
        value +=rounding;

        Serial.print("rounding: ");
        Serial.println(rounding,digits);
        Serial.print("value plus rounding:");
        Serial.println(value,digits);

		//dtostrf(floatVal, 4, 3, charVal);

    }

} ;

extern conversion converter;

//#endif

