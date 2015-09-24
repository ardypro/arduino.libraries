/*
   yeelink api json字串处理单元
*/
#include <SPI.h>
#include <Ethernet.h>
#include "dht11.h"
// for yeelink api
#define APIKEY         "89501b438791e28f74c21035a88fffd2" // replace your yeelink api key here
#define DEVICEID       60 // replace your device ID
#define TEMPERATURESENSORID       54 // replace your sensor ID
#define HUMIDITYSENSORID 3612//
#define DHT11PIN 2
#define _DEBUG_

// assign a MAC address for the ethernet controller.
byte mac[] = { 
  0x00, 0xD0, 0xD0, 0x83, 0x2A, 0xE0 };
byte ip[]={ 
  192,168,1,180};

int t; //temperature
int h; //humidity

int tmp[5]={0,0,0,0,0};
int hmd[5]={0,0,0,0,0};

int index =0;

// initialize the library instance:
EthernetClient client;
char server[] = "api.yeelink.net";   // name address for yeelink API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 20*1000; // delay between 2 datapoints, 30s

int rnd;

dht11 DHT11;

int getLength(int someValue) {
  // there's at least one byte:
  int digits = 1;

  int dividend = someValue /10;
  while (dividend > 0) {
	dividend = dividend /10;
	digits++;
  }
  // return the number of digits:
  return digits;
}

//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
  double A0= 373.15/(273.15 + celsius);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * humidity;
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558-T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity/100);
  double Td = (b * temp) / (a - temp);
  return Td;
}



// this method makes a HTTP connection to the server:
void sendData(int thisData, int sensorID) {
  // if there's a successful connection:
  Serial.print("  ");
  Serial.print(thisData);
  Serial.println(" to be sent");
  String json="";

  client.stop();
  if (client.connect(server, 80)) {
	Serial.println("  connecting api server...");
	// send the HTTP PUT request:

	client.print("POST /v1.0/device/");
	client.print(DEVICEID);

	client.print("/sensor/");
	client.print(sensorID);
	client.print("/datapoints");
	client.println(" HTTP/1.1");
	client.println("Host: api.yeelink.net");
	client.print("Accept: *");
	client.print("/");
	client.println("*");
	client.print("U-ApiKey: ");
	client.println(APIKEY);
	client.print("Content-Length: ");

	// calculate the length of the sensor reading in bytes:
	// 8 bytes for {"value":} + number of digits of the data:
	int thisLength = 10 + getLength(thisData);
	client.println(thisLength);

	client.println("Content-Type: application/x-www-form-urlencoded");
	client.println("Connection: close");
	client.println();

	// here's the actual content of the PUT request:
	client.print("{\"value\":");
	client.print(thisData);
	client.println("}");
#ifdef _DEBUG_
	Serial.print("POST /v1.0/device/");
	Serial.print(DEVICEID);
	Serial.print("/sensor/");
	Serial.print(sensorID);
	Serial.print("/datapoints");
	Serial.println(" HTTP/1.1");
	Serial.println("Host: api.yeelink.net");
	Serial.print("Accept: *");
	Serial.print("/");
	Serial.println("*");
	Serial.print("U-ApiKey: ");
	Serial.println(APIKEY);
	Serial.print("Content-Length: ");

	// calculate the length of the sensor reading in bytes:
	// 8 bytes for {"value":} + number of digits of the data:
	//int thisLength = 10 + getLength(thisData);
	Serial.println(thisLength);

	Serial.println("Content-Type: application/x-www-form-urlencoded");
	Serial.println("Connection: close");
	Serial.println();

	// here's the actual content of the PUT request:
	Serial.print("{\"value\":");
	Serial.print(thisData);
	Serial.println("}");
#endif
	delay(100);

	Serial.println("  Sent already");
	Serial.println(" ");
  }
  else {
	// if you couldn't make a connection:
	Serial.println("  Could not send data, because connection failed");
	Serial.println();
	Serial.println("  closing the connection...");
	Serial.println("");
	client.stop();
  }

  lastConnectionTime = millis();
}




bool readDHT11()  //将函数的返回值类型从float修改为bool，这样可以抛弃读取到的非法数值，比如超时
{
  Serial.println("\n");

  int chk = DHT11.read(DHT11PIN);
  bool rt=false;

  Serial.print("Read sensor: ");
  switch (chk)
  {
  case DHTLIB_OK: 
	Serial.println("OK"); 
	rt=true;
	break;
  case DHTLIB_ERROR_CHECKSUM: 
	Serial.println("Checksum error"); 
	break;
  case DHTLIB_ERROR_TIMEOUT: 
	Serial.println("Time out error"); 
	break;
  default: 
	Serial.println("Unknown error"); 
	break;
  }

//t=DHT11.temperature;
//h=DHT11.humidity;
  if (rt) {
  tmp[index]= DHT11.temperature;
  hmd[index]=DHT11.humidity;
	  }

#ifdef _DEBUG_
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature, 2);

  Serial.print("Temperature (oF): ");
  Serial.println(Fahrenheit(DHT11.temperature), 2);

  Serial.print("Temperature (K): ");
  Serial.println(Kelvin(DHT11.temperature), 2);

  Serial.print("Dew Point (oC): ");
  Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

  Serial.print("Dew PointFast (oC): ");
  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
#endif

  return rt;

  // delay(2000);  
}

void sendTemperature()
{
 sendData( t, TEMPERATURESENSORID);  
}

void sendHumidity()
{
sendData(h, HUMIDITYSENSORID);
}

int readSensors()
{//存在瑕疵，如果读取超时，则返回值就不正确，而不是误差
  if (index<5)
	  {
		if ( readDHT11())  //多个传感器绑在一起，最好能分离
			index ++;
	  }
  else if (index>=5)
	  {
	  t=(tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4])/5;
	  h=(hmd[0]+hmd[1]+hmd[2]+hmd[3]+hmd[4])/5;
	  }

  return 0;
}


void clearSensors()
	{
	index =0;
	for (int i=0; i<5; i++)
		{
		tmp[i]=0;
		hmd[i]=0;
		}
	}


