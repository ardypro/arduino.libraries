#include "lwSimpleHTTPClient.h"
#include "conversion.h"

#define LEWEISERVER "open.lewei50.com"

lwSimpleHTTPClient::lwSimpleHTTPClient(void)
{
	setPostTimeout(5000);
	setAPIVersion("1");
}


lwSimpleHTTPClient::~lwSimpleHTTPClient(void)
{
}


void lwSimpleHTTPClient::post(const char* sensor, char* value)
{
	EthernetClient client;
	_lastPOSTState = false;//TODO:  测试：拔掉ethernet适配器，在看门狗中检查这一句执行情况，看到底整个程序是假死状态还是其它情形。


	unsigned long timeOUt=millis();

	//while (((millis()- timeOUt)< getPostTimeout()) && !lastPostState())
	{
#pragma region IF_CONNECTED_REGION
		if (client.connect("open.lewei50.com", 80))  //这一句好像没有timeout功能，卡死中
		{
			byte len=24+strlen(value)+strlen(sensor);

			client.print("POST /api/V1/Gateway/UpdateSensors/");
			client.print(_deviceref);
			client.print(" HTTP/1.1");
			client.print(LINEBREAK);
			client.print("userkey: ");
			client.print(_userkey);
			client.print(LINEBREAK);
			client.print("Host: open.lewei50.com");
			client.print(LINEBREAK);
			client.print("User-Agent: UNO");
			client.print(LINEBREAK);
			client.print("Content-Length: ");
			client.print(len);
			client.print(LINEBREAK);
			//client.print("Connection: close\r\n");
			client.print(LINEBREAK);

			client.print("[{\"Name\":\"");
			client.print(sensor);
			client.print("\",\"Value\":\"");
			client.print(value);
			client.print("\"}]");
			client.print(LINEBREAK);


			unsigned long now = millis();
			while ((millis()-now) < 10000)
			{
				if (client.connected())
				{
					if (client.find("true"))
						//if (client.find("200 OK"))  //这个可能比上面那个快点，但是需要测试数据格式不正确等情形服务器返回的信息
					{
						client.flush();				
						_lastPOSTState = true;
						_lastPOSTTime=millis();

						break;
					}
				}
				else
				{

				}

			}


		}
		else
		{

		}
#pragma endregion
	}

	delay(WAIT_FOR_SERVER_RESPONSE);
	client.stop();
}

void lwSimpleHTTPClient::setPostTimeout(unsigned int value)
{
	if (value<=5000)		//这个值乱取的
		_postTimeout=5000;
	else
		_postTimeout=value;
}

//http://www.lewei50.com/api/v1/sensor/gethistorydata/你的测量设备id
byte lwSimpleHTTPClient::get(const char* sensor, char* &value, byte length)
{
	EthernetClient client;

	if (client.connect ("open.lewei50.com", 80))
	{
		client.print("GET /api/V1/sensor/gethistorydata/");
		client.print(sensor);
		client.print("?StartTime=&EndTime=&Interval=&Start=0&Limit=1&Order=0 HTTP/1.1");			
		client.print(LINEBREAK);
		client.print("Host: www.lewei50.com");			
		client.print(LINEBREAK);
		client.print("userkey: 029b3884b91e4d00b514158ba1e2ac57");			
		client.print(LINEBREAK);
		client.print("Cache-Control: no-cache");			
		client.print(LINEBREAK);
		client.print(LINEBREAK);



		while (client.connected())
		{
			if (client.available())
			{
				float f;
				if (	client.find("\"value\":"))
				{
					//f=	client.parseFloat();
					//converter.floatToStr(f,value,2);
					client.readBytesUntil('}',value, 20);


					break;
				}

			}
		}


	}

	client.flush();
	client.stop();

	return 0;
}

char* lwSimpleHTTPClient::getAPIVersion()
{
	return _APIVersion;
}

void lwSimpleHTTPClient::sendHeader(EthernetClient *client)
{
	//    Serial.println("sending json");

	client->print("POST /api/V1/Gateway/UpdateSensors/");
	client->print(_deviceref);
	client->print(" HTTP/1.1");			
	client->print(LINEBREAK);
	client->print("userkey: ");
	client->print(_userkey);			
	client->print(LINEBREAK);
	client->print("Host: ");
	client->print(LEWEISERVER);			
	client->print(LINEBREAK);
	client->print("User-Agent: UNO");			
	client->print(LINEBREAK);
	client->print("Content-Length: ");

}

unsigned long lwSimpleHTTPClient::idOfSensor(char* sensor)
{
	unsigned long ret=0;

	for (int i=0; i< MAX_SENSORS_PER_DEVICE; i++)
	{
		if (sensorIDS[i].idName==sensor)
		{
			ret= sensorIDS[i].id;
			break;
		}
	}

	return ret;
}



byte lwSimpleHTTPClient::getAllSensorIDOfDevice(char* device)
{
	char* idFlag ="\"id\": ";
	char* idNameFlag = "\"idName\": \"";

	byte idx=0;

	for (idx=0; idx< MAX_SENSORS_PER_DEVICE; idx++)
	{
		sensorIDS[idx].id =0;
		sensorIDS[idx].idName=NULL;
	}

	EthernetClient client;
	unsigned long ret=0;
	if (client.connect(LEWEISERVER,80))
	{
		client.print("GET /api/V1/user/getSensorsWithGateway HTTP/1.1");			
		client.print(LINEBREAK);
		client.print("Host: www.lewei50.com");			
		client.print(LINEBREAK);
		client.print("userkey: ");
		client.print(_userkey);			
		client.print(LINEBREAK);
		client.print("Cache-Control: no-cache");			
		client.print(LINEBREAK);
		client.print(LINEBREAK);


		delay(10);
		idx=0;
		while (client.connected())
		{
			char c[20]={0};

			strcat(c, idNameFlag);
			strcat(c,device);		//c=>"idName": "01
			strcat(c,"\"");


			if (client.available())
			{
				if (client.findUntil(c,"\n\r"))	//寻找网关
				{
					Serial.println();
					Serial.print("found device of ");Serial.println(c);
					Serial.println();

					while (client.find(idFlag))
					{
						sensorIDS[idx].id = client.parseInt();
						client.find(idNameFlag);
						char* name;
						client.readBytesUntil('"',name,20);
						sensorIDS[idx].idName =name;

						Serial.println();
						Serial.print("found ");Serial.println(idx+1);
						Serial.print("	idx:");Serial.println(idx);
						Serial.print("	id:");Serial.println(sensorIDS[idx].id);
						Serial.print("	name:");Serial.println(sensorIDS[idx].idName);

						idx++;

						if (client.find("\"sensors\": ["))  
							break;
					}
				}
				else
				{
					Serial.println();
					Serial.print("could not find any device of:");Serial.println(c);
					Serial.println();
				}
				//break;
			}
		}
	}
	ret=idx;
	delay(WAIT_FOR_SERVER_RESPONSE);
	client.stop();

	return ret;
}

void lwSimpleHTTPClient::selectDevice(const char* device)
{
	IOTFramework::selectDevice(device);

	Serial.print("selecting device :  ");Serial.print(device);Serial.print("/");Serial.println((char*)device);

}


