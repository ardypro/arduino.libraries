#pragma once

#include "IOTFrameworkOverW5100.h"

//#define DEBUGGING_LEWEI_POST
#define DEBUGGING_LEWEI_GET

#define MAX_SENSORS_PER_DEVICE 10	//每个设备允许的传感器数

class lwSimpleHTTPClient : public IOTFrameworkOverW5100
{
public:
	lwSimpleHTTPClient(void);
	virtual ~lwSimpleHTTPClient(void);

	virtual char* getAPIVersion();

	virtual void post(const char* sensor, char* value);
	virtual void setPostTimeout(unsigned int value);

	virtual byte get(const char* sensor, char* &value, byte length);

	//************************************
	// Method:    selectDevice	覆盖此方法，读取ID跟sensorname之间的映射关系
	// FullName:  lwSimpleHTTPClient::selectDevice
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * device
	//************************************
	virtual void selectDevice(const char* device);
private:
	struct SENSORIDSTRUCT
	{
		unsigned long id;
		char* idName;
	};

	SENSORIDSTRUCT sensorIDS[MAX_SENSORS_PER_DEVICE];

	void sendHeader(EthernetClient *client);
	unsigned long idOfSensor(char* sensor);

	//************************************
	// Method:    getSensorIDs	返回传感器ID列表
	// FullName:  lwSimpleHTTPClient::getSensorIDs
	// Access:    private 
	// Returns:   byte
	// Qualifier:
	// Parameter: char * device
	//************************************
	byte getAllSensorIDOfDevice(char* device);
};

