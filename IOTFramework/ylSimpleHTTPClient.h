#pragma once

#include "IOTFrameworkOverW5100.h"


//#define		DEBUGGING_YEELINK_POST
#define		DEBUGGING_YEELINK_GET


/*
	yeelink API 
	
	v1.0

	Devices		http://www.yeelink.net/developer/apidoc/9

		�����豸		http://api.yeelink.net/v1.0/devices													POST����
		�༭�豸		http://api.yeelink.net/v1.0/device/<device_id>										PUT����
		�����豸		http://api.yeelink.net/v1.0/devices													GET����
		�鿴�豸		http://api.yeelink.net/v1.0/device/<device_id>										GET����
		ɾ���豸		http://api.yeelink.net/v1.0/device/<device_id>										DELETE����

	Sensors		http://www.yeelink.net/developer/apidoc/10
		
		����������	http://api.yeelink.net/v1.0/device/<device_id>/sensors								POST����
		�༭������	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					PUT����
		���д�����	http://api.yeelink.net/v1.0/device/<device_id>/sensors								GET����
		�鿴������	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					GET����
		ɾ��������	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					DELETE����

	Datapoints	http://www.yeelink.net/developer/apidoc/11
		
		�������ݵ�	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoints		POST����
		�༭���ݵ�	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	PUT����
		�鿴���ݵ�	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	keyΪ��ѡ��GET����
		ɾ�����ݵ�	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	DELETE/PUT���� 

	History Data	http://www.yeelink.net/developer/apidoc/13
		��ʷ����		http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>.json?start=<timestamp>&end=<timestamp>&interval=<interval>&page=<page>
					GET����

					
	API Key		http://www.yeelink.net/developer/apidoc/15
		����key		/v1.0/user/apikey?username=<username>&pass=<pass>									GET����

*/

class ylSimpleHTTPClient :
	public IOTFrameworkOverW5100
{
public:
	ylSimpleHTTPClient(void);
	virtual ~ylSimpleHTTPClient(void);

	virtual void post(const char* sensor, char* value);
	virtual void setPostTimeout(unsigned int value);

	virtual byte get(const char* sensor, char* &value, byte length);
	
private:
	
};

