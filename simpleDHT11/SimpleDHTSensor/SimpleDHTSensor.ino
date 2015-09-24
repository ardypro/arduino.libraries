#include "simpleDHT11.h"

simpleDHT11 dht(3);

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	float h;
	float t;
	byte err;
	if ((err=dht.read(h,t))==0)
	{
		Serial.print("humidity:\t");
		Serial.println(h);
		Serial.print("temperature:\t");
		Serial.println(t);

	}
	else
	{
		Serial.print("reading error:\t");
		Serial.println(err);
	}
	Serial.println();

	delay(1000);
}