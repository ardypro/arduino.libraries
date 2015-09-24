/*
 * Example file showing use of the DS18B20 library
 *
 * v0.2 - 2011-05-11 
 */

#include <OneWire.h>
#include <DS18B20.h>

#define numberOfSensors 3  //number of sensors to check

DS18B20 myDS(6);       //create object, passin the pin to use for the OneWire bus

static byte sensorArray[numberOfSensors][8];  //creating an array to hold the 8 byte addresses of all sensors

void setup()
{
  Serial.begin(9600);
  
  myDS.init(numberOfSensors);  //run the initialise function, passing the number of sensors.
  
  myDS.getAddresses(sensorArray);  //get all addresses and pass back into the array we created

  for(int i = 0; i < numberOfSensors; i++)  //for each sensor...
  {
    for(int j = 0; j < 8; j++)   //... and for each byte of the address of each sensor...
    {
      Serial.print(sensorArray[i][j], HEX);  //print the byte
      if(j < 7)
      {
        Serial.print(":");
      }
      else
      {
        Serial.print("  --  ");
        Serial.print(myDS.getTemp(sensorArray[i]));  //when the address has been printed, get the temperature
                                                     //by passing the address of the sensor (returns a float)
        Serial.println("");
      } 
    }
  }
}

void loop()
{
  
}
