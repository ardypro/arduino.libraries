#pragma once

/*
******************基于W5100模块的物联网服务接口*****************

Data Created:		2014-04-18



*/

#include "iotframework.h"

#define Interface public

class IOTFrameworkOverW5100 : public IOTFramework
{
public:

	IOTFrameworkOverW5100(void)
	{
	}

	virtual ~IOTFrameworkOverW5100(void)
	{
	}

	virtual bool initialize(void)
	{
		byte mac[] ={0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
		bool ret=  Ethernet.begin(mac);

		return ret;
	}

	virtual bool initialize(uint8_t* MAC)
	{
		return Ethernet.begin(MAC);
	}

	virtual bool initialize(IPAddress localip)
	{
		byte mac[] ={0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
		Ethernet.begin(mac, localip);

		return (Ethernet.localIP()==localip);
	}

	virtual bool initialize(uint8_t* MAC, IPAddress localip)
	{
		Ethernet.begin(MAC,localip);
		return (Ethernet.localIP()=localip);
	}


};

