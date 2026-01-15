#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAndroid.h"
#include <android/looper.h>
#include <android/sensor.h>
#include <unistd.h>

IO::SensorAndroid::SensorAndroid(void *sensor, void *sensorMgr, Int32 id)
{
	this->sensor = sensor;
	this->sensorMgr = sensorMgr;
	this->id = id;
	this->queue = ASensorManager_createEventQueue((ASensorManager*)this->sensorMgr, ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS), id, 0, 0);
	if (this->queue == 0)
	{
//		printf("SensorAndroid queue = 0\r\n");
	}
}

IO::SensorAndroid::~SensorAndroid()
{
	if (this->queue)
	{
		ASensorManager_destroyEventQueue((ASensorManager*)this->sensorMgr, (ASensorEventQueue*)this->queue);
		this->queue = 0;
	}
}

Text::CString IO::SensorAndroid::GetVendor()
{
	return Text::CString::FromPtr((const UTF8Char*)ASensor_getVendor((const ASensor*)this->sensor));
}

Text::CString IO::SensorAndroid::GetName()
{
	return Text::CString::FromPtr((const UTF8Char*)ASensor_getName((const ASensor*)this->sensor));
}

Bool IO::SensorAndroid::EnableSensor()
{
	if (this->queue == 0)
		return false;
	int ret = ASensorEventQueue_enableSensor((ASensorEventQueue*)this->queue, (const ASensor*)this->sensor);
//	printf("EnableSensor return %d\r\n", ret);
	return ret == 0;
}

Bool IO::SensorAndroid::DisableSensor()
{
	if (this->queue == 0)
		return false;
	int ret = ASensorEventQueue_disableSensor((ASensorEventQueue*)this->queue, (const ASensor*)this->sensor);
//	printf("DisableSensor return %d\r\n", ret);
	return ret == 0;
}

Bool IO::SensorAndroid::GetSensorEvent(void *sensorEvt)
{
	if (this->queue == 0)
		return false;
	IntOS i = 10;
	while (i-- > 0)
	{
		int id = ALooper_pollAll(1000, 0, 0, 0);
		if (id == this->id)
		{
			break;
		}
	}
	int ret = ASensorEventQueue_getEvents((ASensorEventQueue*)this->queue, (ASensorEvent*)sensorEvt, 1);
//	printf("getEvents return %d\r\n", ret);
	return ret != 0;
}
