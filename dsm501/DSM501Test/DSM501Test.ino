//#include <Arduino.h>

/*
  Turns on an LED on for one second, then off for one second, repeatedly.
 */
#include "dsm501.h"

#include <LeweiClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> //BH1750 IIC Mode


#define LW_USERKEY "2c2a9948d4c049c18560ddbfb46930d8"
#define LW_GATEWAY "01"

#define MY_NAME    "O1"
#define MY_DESC    "virtual gate"
char my_addr[50]="http://192.168.1.221/api";
int port =8889;



LeWeiClient *lwc;

dsm501 DSM501A;

void setup()
{
    Serial.begin(9600);
    DSM501A.setDatapin(8);
    DSM501A.enableFilter();


    /*
  Serial.begin(9600);

  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);
  DSM501A.setDatapin(8);

  String stringOne;
  Serial.begin(9600);

  uint8_t mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED        };
#if 0
  IPAddress  myip(192, 168, 1, 33);
  IPAddress  dnsip(202,103,225,68); //get this ip from local isp
  IPAddress  gateway(192, 168, 1, 1);
  Ethernet.begin(mac, myip, dnsip, gateway);
#else
  Serial.println("DHCP in process !");
  if (Ethernet.begin(mac) == 0)
  {
    Serial.print(F("Failed to configure Ethernet using DHCP\n"));
  }
  else
  {
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print(".");
    }
    Serial.println();
    Serial.println(F("Ethernet configuration OK\n"));
    stringOne="http://";
    stringOne+=Ethernet.localIP()[0];
    stringOne+=".";
    stringOne+=Ethernet.localIP()[1];
    stringOne+=".";
    stringOne+=Ethernet.localIP()[2];
    stringOne+=".";
    stringOne+=Ethernet.localIP()[3];
    stringOne+=":";
    stringOne+=port;
    stringOne+="/api";
    Serial.println(stringOne);
    stringOne.toCharArray(my_addr, 50);
  }
#endif

  // hope no exception here
  lwc = new LeWeiClient(LW_USERKEY, LW_GATEWAY,MY_NAME, MY_DESC, my_addr, (LeWeiClient::flag)((LeWeiClient::isControlled)|(LeWeiClient::internetAvailable)));
  // lwc = new LeWeiClient(LW_USERKEY, LW_GATEWAY,MY_NAME, MY_DESC, my_addr, (LeWeiClient::flag)(LeWeiClient::isControlled));

  // lwc->registerSensor(the_UVSensor);

  Serial.print(lwc->nrSensors());
  Serial.println(F(" sensors registered."));
  Serial.print(lwc->nrActuators());
  Serial.println(F(" actuators registered."));

  lwc->initDevices();

  Serial.println(F("upload gateway info to server"));
  int retry = 10;
  while (lwc->uploadInfo() < 0 && --retry)
  {
    delay(500);

    Serial.print("retry: ");
    Serial.println(retry);
  }
  if (!retry)
    Serial.println(F("uploadInfo failed in 10 times"));
  else
    Serial.println(F("uploadInfo done"));
*/
}

void loop()
{

    int ov, fv;
    DSM501A.getParticles(ov,fv);

    Serial.print(ov);
    Serial.print("\t");
    Serial.println(fv);
    Serial.println(" ");

  //Serial.println("Hello world!");
//
//  delay(1000);              // wait for a second
//  digitalWrite(13, HIGH);   // set the LED on
//  delay(1000);              // wait for a second
//  digitalWrite(13, LOW);    // set the LED off
//
//  Serial.println(" ");
//  Serial.print ("Concentration is: ");
//  Serial.println(DSM501A.Getparticles());
//  Serial.println(" ");

 // lwc->append("p1",DSM501A.getParticles());
 // lwc->scanSensors();
}


