#pragma once

#include "IOTFrameworkOverW5100.h"


//#define		DEBUGGING_YEELINK_POST
#define		DEBUGGING_YEELINK_GET


/*
	yeelink API 
	
	v1.0

	Devices		http://www.yeelink.net/developer/apidoc/9

		创建设备		http://api.yeelink.net/v1.0/devices													POST方法
		编辑设备		http://api.yeelink.net/v1.0/device/<device_id>										PUT方法
		罗列设备		http://api.yeelink.net/v1.0/devices													GET方法
		查看设备		http://api.yeelink.net/v1.0/device/<device_id>										GET方法
		删除设备		http://api.yeelink.net/v1.0/device/<device_id>										DELETE方法

	Sensors		http://www.yeelink.net/developer/apidoc/10
		
		创建传感器	http://api.yeelink.net/v1.0/device/<device_id>/sensors								POST方法
		编辑传感器	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					PUT方法
		罗列传感器	http://api.yeelink.net/v1.0/device/<device_id>/sensors								GET方法
		查看传感器	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					GET方法
		删除传感器	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>					DELETE方法

	Datapoints	http://www.yeelink.net/developer/apidoc/11
		
		创建数据点	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoints		POST方法
		编辑数据点	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	PUT方法
		查看数据点	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	key为可选，GET方法
		删除数据点	http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>/datapoint/<key>	DELETE/PUT方法 

	History Data	http://www.yeelink.net/developer/apidoc/13
		历史数据		http://api.yeelink.net/v1.0/device/<device_id>/sensor/<sensor_id>.json?start=<timestamp>&end=<timestamp>&interval=<interval>&page=<page>
					GET方法

					
	API Key		http://www.yeelink.net/developer/apidoc/15
		请求key		/v1.0/user/apikey?username=<username>&pass=<pass>									GET方法

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

