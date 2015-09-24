#pragma once

/*

*********************物联网服务框架******************

HISTROY
2014-04-18	将IOTFramework接口中的initialize()方法改为纯虚方法，增加IOTFrameworkOnW5100接口，其中实现了initialize()方法
ylSimpleHTTPClient类的父类从IOTFramework改为IOTFrameworkOnW5100，因为其实现代码并不支持ENC28J60模块

*/

#include <Ethernet.h>
#include <SPI.h>
#include "conversion.h"

#define WAIT_FOR_SERVER_RESPONSE 1000
#define LINEBREAK "\r\n"


enum POSTSTATE
{
	SUCCESSFUL,
	FAILED,
	UNSURE
};

class IOTFramework
{
public:

	IOTFramework(char* key)
	{
		_lastPOSTState = false;
		_lastPOSTTime = 0;
		setUserkey(key);
	}

	IOTFramework(void)
	{
		_lastPOSTState = false;
		_lastPOSTTime = 0;
	}


	virtual ~IOTFramework(void)
	{
	}

	virtual void setUserkey(const char* key)
	{
		_userkey = key;
	}


	virtual void selectDevice(const char* device)
	{
		_deviceref = device;
	}


protected:
	const char*		_userkey;
	const char*		_deviceref;
	bool			_lastPOSTState;
	unsigned long	_lastPOSTTime;
	unsigned int	_postInterval;
	unsigned int	_postTimeout;
	char*			_APIVersion;

public:
	//************************************
	// Method:    initialize	初始化框架，设置IP、MAC地址，以及联网
	// FullName:  IOTFramework::initialize
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void
	//************************************
	virtual bool  initialize(void) = 0;
	virtual bool  initialize(uint8_t* MAC) = 0;						//同一个局域网中存在多个网络扩展板时，需要有不同的MAC地址
	virtual bool  initialize(IPAddress localip) = 0;
	virtual bool  initialize(uint8_t* MAC, IPAddress localip) = 0;

	//************************************
	// Method:    post	上传布尔类型数据
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: bool value
	//************************************
	virtual void  post(const char* sensor, bool value)
	{
		char b[MAX_BOOL_LEN];
		converter.boolToStr(value, b);

		post(sensor, b);
	}

	//************************************
	// Method:    post	上传整数
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: int value
	//************************************
	virtual void post(const char* sensor, int value)
	{
		char i[MAX_INT_LEN];
		converter.intToStr(value, i);

		post(sensor, i);

	}



	//************************************
	// Method:    post	上传浮点数，默认小数位为2
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: float value
	// Parameter: byte precision
	//************************************
	virtual void post(const char* sensor, float value, byte precision = 2)
	{
		char f[MAX_DOUBLE_LEN];
		converter.doubleToStr(value, f, precision);

		post(sensor, f);
	}


	//************************************
	// Method:    post	上传长整数
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: long value
	//************************************
	virtual void post(const char* sensor, long value)
	{
		char l[MAX_LONG_LEN];
		converter.longToStr(value, l);

		post(sensor, l);
	}

	//************************************
	// Method:    post	上传无符号位长整数
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: unsigned long value
	//************************************
	virtual void post(const char* sensor, unsigned long value)
	{
		char ul[MAX_LONG_LEN];
		converter.ulongToStr(value, ul);

		post(sensor, ul);
	}

	//************************************
	// Method:    post	上传字符串类型数据，以及泛型等用字符串封装的数据
	// FullName:  IOTFramework::post
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: char * value
	//************************************
	virtual void post(const char* sensor, char* value) = 0;

	//************************************
	// Method:    lastPostState
	// FullName:  IOTFramework::lastPostState
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier: //前一次发送结果
	//************************************
	virtual bool lastPostState()					//前一次发送结果
	{
		return _lastPOSTState;
	}

	//************************************
	// Method:    lastSuccessfulPost
	// FullName:  IOTFramework::lastSuccessfulPost
	// Access:    virtual public 
	// Returns:   unsigned long
	// Qualifier: //距离前一次正确发送时间间隔（ms）
	//************************************
	virtual unsigned long lastSuccessfulPost()   //距离前一次正确发送时间间隔（ms）
	{
		return _lastPOSTTime;
	}
	//virtual bool get(const char* sensor, bool &value, char* timeStr=""/*最新数值*/)=0;

	//************************************
	// Method:    setPostInterval	设置上传间隔（ms）
	// FullName:  IOTFramework::setPostInterval
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: unsigned int value
	//************************************
	virtual void setPostInterval(unsigned int value)
	{
		_postInterval=value;
	}

	//************************************
	// Method:    getPostInterval
	// FullName:  IOTFramework::getPostInterval
	// Access:    virtual public 
	// Returns:   unsigned int
	// Qualifier:
	//************************************
	virtual unsigned int getPostInterval()
	{
		return _postInterval;
	}

	//************************************
	// Method:    setPostTimeout
	// FullName:  IOTFramework::setPostTimeout
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: unsigned int value
	//************************************
	virtual void setPostTimeout(unsigned int value)=0;

	//************************************
	// Method:    getPostTimeout
	// FullName:  IOTFramework::getPostTimeout
	// Access:    virtual public 
	// Returns:   unsigned int
	// Qualifier:
	//************************************
	virtual unsigned int getPostTimeout()
	{
		return _postTimeout;
	}


public:		//从服务器GET数据

	//************************************
	// Method:    get
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: char *  & value
	// Parameter: byte length
	//************************************
	virtual byte get(const char* sensor, char* &value, byte length)=0;

	//************************************
	// Method:    get	从服务器获取布尔类型数据
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: bool & value
	//************************************
	virtual byte get(const char* sensor, bool &value)
	{
		char* v = (char*) malloc(sizeof(value));
		byte ret;

		ret=	get(sensor, v,1);
		if (v[0]=='1')
			value=true;
		else
			value=false;

		free(v);

		return ret;
	}

	//************************************
	// Method:    get	从服务器获取整数类型数据
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: int & value
	//************************************
	virtual byte get(const char* sensor, int &value)
	{
		byte size= sizeof(value);
		byte ret=0;
		char* v=(char*) malloc(size);
		ret=get(sensor, v,size);
		value= atoi(v);
		free(v);
		return ret;
	}

	//************************************
	// Method:    get	从服务器获取无符号整数
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: unsigned int & value
	//************************************
	virtual byte get(const char* sensor, unsigned int &value)
	{
		byte size = sizeof(value);
		byte ret=0;
		char* v= (char*) malloc(size);
		ret=get(sensor,v,size);
		//没发现字符串转uint的函数，暂时用long类型替代
		value=atol(v);
		free(v);
		return ret;
	}

	//************************************
	// Method:    get
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: long & value
	//************************************
	virtual byte get(const char* sensor, long &value)
	{
		byte size = sizeof(value);
		byte ret=0;
		char* v= (char*) malloc(size);
		ret=get(sensor,v,size);
		value=atol(v);
		free(v);
		return ret;
	}

	//************************************
	// Method:    get
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: unsigned long & value
	//************************************
	virtual byte get(const char* sensor, unsigned long &value)
	{
		byte size = sizeof(value);
		byte ret=0;
		char* v= (char*) malloc(size);
		ret=get(sensor,v,size);
		//此次应该有问题，atol是long，跟ulong的数据不一样
		value=atol(v);
		free(v);
		return ret;
	}

	//************************************
	// Method:    get
	// FullName:  IOTFramework::get
	// Access:    virtual public 
	// Returns:   byte
	// Qualifier:
	// Parameter: const char * sensor
	// Parameter: float & value
	// Parameter: byte precision
	//************************************
	virtual byte get(const char* sensor, float &value, byte precision=2)
	{
		//依据微软MSDN示例
		//http://msdn.microsoft.com/en-us/library/hc25t012.aspx

		byte size =10+2+precision;  //负号、小数点、小数位

		byte ret=0;
		char* v= (char*) malloc(size);

		ret=get(sensor,v,size);

		value=atof(v);  
		free(v);

		return ret;
	}

	//************************************
	// Method:    setAPIVersion
	// FullName:  IOTFramework::setAPIVersion
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: char * value
	//************************************
	virtual void setAPIVersion(char* value)
	{
		//版本号不能带v字符
		if (value[0]=='v' || value[0]=='V')
			_APIVersion=&value[1];
		else
			_APIVersion=value;
	}

	//************************************
	// Method:    getAPIVersion
	// FullName:  IOTFramework::getAPIVersion
	// Access:    virtual public 
	// Returns:   char*
	// Qualifier:
	//************************************
	virtual char* getAPIVersion()
	{
		return _APIVersion;
	}

};

