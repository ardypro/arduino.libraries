/*
Am2302Sensor
Reads humidity and temperature from Am2302 sensor.

  Copyright (c) 2013 Odd Arild Olsen

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  */

#include <Arduino.h>
#include "Am2302Sensor.h"


static Am2302Sensor *instancePointer = NULL;

/* Constructor */
Am2302Sensor::Am2302Sensor(uint8_t virtualpin) 
{
  sensorpin= digitalPinToBitMask(virtualpin);
  sensorport= digitalPinToPort(virtualpin);
  pin= virtualpin;
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);// pullup on
}

Am2302Sensor::~Am2302Sensor()
{
  instancePointer= NULL;
} 

/* After you have constructed an Am2302Sensor object you can start a 
 new measurement by calling this function
*/
Am2302Sensor::errcode Am2302Sensor::startMeasurement(void)
{
  uint8_t  i;
  errcode r;
  int16_t *ip;
  r= startReadBitstring();
  return r;
}

/* And then at a later moment get the integer results of the
   measurement back. The integer results are 10 times the humidity (%)
   and temperature values (deg C).  See the Am2302Sensor.h file for
   definitions of the returned error codes.
 */
Am2302Sensor::errcode Am2302Sensor::getResult(int16_t *hum, int16_t *temp)
{
  errcode r;
  if((EIMSK &(1<<INT6)) != 0) return BUSY;
  instancePointer= NULL;
  while(pinState()==LOW);
  r= convertResult(hum, temp);
  if(r!=OK) {return PARITY_ERROR;} // check sum error
  return OK;
}

/* or get textual results of the measurement back
   The string results are formatted with decimal points and represent 
   humidity (%) and temperature values (deg C).
   See the Am2302Sensor.h file for definitions of the returned error codes.
 */
Am2302Sensor::errcode Am2302Sensor::getResult(char *hum, char *temp)
{
  errcode r;
  int16_t h, t;
  r= getResult(&h, &t);
  if(r != OK) return r;
  toString(h, hum);
  toString(t, temp);
  return OK;
}


/* Format an integer humidity or temperature value into a formatted
string with a decimal point 
*/
void  Am2302Sensor::toString(int16_t i, char *s)
{
  char *cp, *cp2, r[10], neg=0, addz= 0;
  cp=r+1;
  if(i<0) {neg=1; i= -i;}
  if(i<9) addz= 1;
  do  {
    *cp++= i%10 + '0';
    i /= 10;
    if(i==0) break;
  } while(i != 0);
  if(addz != 0) *cp++= '0';
  if(neg!=0) *cp++= '-';
  cp--;
  r[0]= r[1];
  r[1]= '.';
  //while(*s++= *cp++);
  while(cp >= r) {
    *s++= *cp--;
  }
  *s= '\0';
}

/* This function is called by the interrupt service routine and keeps
track of the pulse widths and detect 0 and 1 pulses. Store the
detected bits in the result array bytes. This function must be
modified if the signal is applied to another pin than number 7. This function
must be public to be accessible through the instance pointer. */

void  Am2302Sensor::irqHandler(void)
{
  int16_t width;
  uint8_t *byte;
  
  PORTD |= (1<<5);// yellow led on
  if(pinState()==LOW) {
    end=  (int16_t)TCNT0;
    sei(); // we've captured the counter, so enable other interrupts
    byte= bytes + ((bitcounter/8)^1);
    *byte = (*byte)<<1;
    width= end - start;
    if(width < 0) width += 256;
    if(width > 50/4) *byte |= 1; 
    bitcounter++;
    if(bitcounter>=40) {
      EIMSK &= ~(1<<INT6);
    }
  }
  else {
    start= (int16_t)TCNT0;
    sei();
  }
  PORTD &= ~(1<<5); // yellow led off

}

/*-------------- Protected functions -------------------*/

/* This function sends the start pulse to the sensor and
sets up interrupt to register raising and falling edges
on pin 7
*/

Am2302Sensor::errcode Am2302Sensor::startReadBitstring(void)
{
  uint8_t i;
  uint32_t now;
  DDRB |= 1; // yellow LED on the Pro Micro
  PORTB |= 1;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);// activation pulse
  delay(1); // 1 msec
  while(TCNT0 >10); // sync with counter overflow
  digitalWrite(pin, HIGH);// term activation pulse
  pinMode(pin, INPUT); 
  while(pinState() != LOW)  {
    if(TCNT0 >200) { // timeout about 190 usec
      return NO_DEVICE;
    }
  }
  // sensor responded
  cli();
  instancePointer= this;
  bitcounter= 0;
  // skip first pulse
  while(pinState() == LOW);
  while(pinState() != LOW);
  /* start the interrupt on positive and negtive transitions. You must
     change the next three lines if the signal is applied to another pin than
     number 7 
  */
  EICRB |= 1<<ISC60;
  EIMSK |= 1<<INT6;
  EIFR  = 1<<INTF6;
  sei(); // go, interrupt, go
  return OK;
}

/* This function converts the raw data from the sensor into
   10 times humidity and deg C.
*/
Am2302Sensor::errcode Am2302Sensor::convertResult(int16_t *hum, int16_t *temp)
{
  uint8_t cs= 0, neg=0, i;
  int16_t t;
  for(i=0; i<4; i++) cs += bytes[i];
  if( cs != bytes[5]) {
    *hum= 0;
    *temp= 0;
    return PARITY_ERROR;
  }
  *hum= *(int16_t*)&bytes[0];
  t= *(int16_t*)&bytes[2];
  if((t & 0x8000) != 0) {
    t= -(t & 0x7fff);
  }
  *temp= t;
  return OK;
}

/* a function that determines if the input pin from the sensor is high or
low when called 
*/
inline uint8_t  Am2302Sensor::pinState(void) {
  return (*portInputRegister(sensorport)) & sensorpin;
}


/*------------- ISR, not a member of the class ---------------------*/

/* interrupt service routine for the sensor signal. Must be rewritten if
the signal is applied to another pin than number 7. Call the class isr via 
the instance pointer.
*/ 
ISR(INT6_vect)
{
  if (instancePointer != 0) {
    instancePointer->irqHandler();
  }
}
