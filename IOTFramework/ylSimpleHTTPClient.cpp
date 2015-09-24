#include "ylSimpleHTTPClient.h"

const char* APIServer = "api.yeelink.net";

ylSimpleHTTPClient::ylSimpleHTTPClient(void)
{
	setPostTimeout(5000);
	setAPIVersion("1.0");
}


ylSimpleHTTPClient::~ylSimpleHTTPClient(void)
{
}


void ylSimpleHTTPClient::post(const char* sensor, char* value)
{
	EthernetClient client;
	_lastPOSTState = false;//TODO:  测试：拔掉ethernet适配器，在看门狗中检查这一句执行情况，看到底整个程序是假死状态还是其它情形。

#if defined DEBUGGING_YEELINK_POST
	Serial.println();
	Serial.print("ylSimpleHTTPClient's post(*,*) (");
	Serial.print(sensor);
	Serial.print(",");
	Serial.print(value);
	Serial.println(") executing...");
	Serial.println();
#endif

	unsigned long timeOUt=millis();

	//while (((millis()- timeOUt)< getPostTimeout()) && !lastPostState())  //执行了10次！
	{
#pragma region IfConnected
		if (client.connect(APIServer, 80))  //这一句好像没有timeout功能，卡死中
		{			//////////////////////////////////////////
			client.print("POST /v");
			client.print(getAPIVersion());
			client.print("/device/");
			client.print(_deviceref);
			client.print("/sensor/");
			client.print(sensor);
			client.print("/datapoints HTTP/1.1");			
			client.print(LINEBREAK);
			client.print("Host: ");
			client.print(APIServer);			
			client.print(LINEBREAK);
			client.print("Accept: *");
			client.print("Accept: /");
			client.print("Accept: *");			
			client.print(LINEBREAK);
			client.print("U-ApiKey: ");
			client.print(_userkey);			
			client.print(LINEBREAK);
			client.print("Content-Length: ");
			client.print(10+strlen(value));			
			client.print(LINEBREAK);
			client.print(LINEBREAK);
			client.print("{\"value\":");
			client.print(value);
			client.print("}");			
			client.print(LINEBREAK);
			client.print(LINEBREAK);

			delay(WAIT_FOR_SERVER_RESPONSE); //等待服务器返回

			
			while (client.connected())
			{
				if (client.find("HTTP/1.1 200 OK"))
				{
					client.flush();				
					_lastPOSTState = true;
					_lastPOSTTime=millis();

					break;
				}
			}

		}
#pragma endregion
	}



	delay(WAIT_FOR_SERVER_RESPONSE);
	client.stop();
}

void ylSimpleHTTPClient::setPostTimeout(unsigned int value)
{
	if (value<=5000)
		_postTimeout=5000;
	else
		_postTimeout=value;
}

byte ylSimpleHTTPClient::get(const char* sensor, char* &value, byte length)
{

	EthernetClient client;
	byte ret=0;


	if (client.connect(APIServer, 80))  //这一句好像没有timeout功能，卡死中
	{
		client.print ("GET /v");
		client.print(getAPIVersion());
		client.print("/device/");
		client.print(_deviceref);
		client.print("/sensor/");
		client.print(sensor);
		client.print("/datapoints HTTP/1.1");			
		client.print(LINEBREAK);
		client.print("Host: ");
		client.print(APIServer);			
		client.print(LINEBREAK);
		client.print("Accept: *");
		client.print("/");
		client.print("*");			
		client.print(LINEBREAK);
		client.print("U-ApiKy: ");
		client.print(_userkey);			
		client.print(LINEBREAK);
		//client.print("Connection: close");			client.print(LINEBREAK);
		client.print(LINEBREAK);

		delay(WAIT_FOR_SERVER_RESPONSE); //等待服务器返回



		//***********************注意，1.0版本API协议中，返回的数据没有传感器或者设备信息，数据是否真实无从可考**********************************
		while (client.connected())
		{
			if ( client.find("\"value\":"))
			{
				client.readBytesUntil('}', value, length);

				break;
			}
		}
	}


	delay(WAIT_FOR_SERVER_RESPONSE);
	client.stop();

	return strlen(value);

}



