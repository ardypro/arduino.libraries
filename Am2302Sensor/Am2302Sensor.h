/*
 reads humidity and temperature from am2302 sensor.
  */

#ifndef Am2302Sensor_H 
#define Am2302Sensor_H 
#include <Ardunio.h>
#include <stdint.h>

class Am2302Sensor{
public: 
	typedef enum errcode {
		OK,
		NO_DEVICE, 
		PARITY_ERROR, 
		BUSY
	} errcode;

	Am2302Sensor(uint8_t virtualpin);
	~Am2302Sensor();
	Am2302Sensor::errcode startMeasurement(void);
	Am2302Sensor::errcode getResult(int16_t *hum, int16_t *temp);
	Am2302Sensor::errcode getResult(char *hum, char *temp);
	void toString(int16_t i, char *s);
	void irqHandler(void);
protected:
	Am2302Sensor::errcode startReadBitstring(void);
	Am2302Sensor::errcode convertResult(int16_t *hum, int16_t *temp);
	inline uint8_t pinState(void);

	uint8_t sensorport;
	uint8_t sensorpin;
	uint8_t pin;
	int16_t start, end;
	volatile uint8_t bitcounter;
	uint8_t bytes[6];
};

#endif
