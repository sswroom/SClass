#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAccelerometerAndroid.h"
#include "IO/SensorManager.h"
#include "IO/SensorMagnetometerAndroid.h"
#include "IO/SensorPressureAndroid.h"
#include "IO/SensorUnknownAndroid.h"

#include <stdio.h>
#include <android/sensor.h>

typedef struct
{
	ASensorManager *mgr;
	ASensorList sensorList;
	OSInt sensorCnt;
} ClassData;

IO::SensorManager::SensorManager()
{
	ClassData *me = MemAlloc(ClassData, 1);
	this->clsData = me;
	me->mgr = ASensorManager_getInstance();
	me->sensorList = 0;
	me->sensorCnt = 0;
	if (me->mgr)
	{
		me->sensorCnt = ASensorManager_getSensorList(me->mgr, &me->sensorList);
	}
}

IO::SensorManager::~SensorManager()
{
	ClassData *me = (ClassData*)this->clsData;
	MemFree(me);
}

OSInt IO::SensorManager::GetSensorCnt()
{
	ClassData *me = (ClassData*)this->clsData;
	return me->sensorCnt;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(OSInt index)
{
	ClassData *me = (ClassData*)this->clsData;
	if (index < 0 || index >= me->sensorCnt)
	{
		return IO::Sensor::ST_UNKNOWN;
	}
	switch (ASensor_getType(me->sensorList[index]))
	{
	case ASENSOR_TYPE_ACCELEROMETER:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
#endif
		return IO::Sensor::ST_ACCELEROMETER;
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_PRESSURE:
		return IO::Sensor::ST_PRESSURE;
#endif
	case ASENSOR_TYPE_MAGNETIC_FIELD:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
#endif
		return IO::Sensor::ST_MAGNETOMETER;
	default:
		return IO::Sensor::ST_UNKNOWN;
	}

}

IO::Sensor *IO::SensorManager::CreateSensor(OSInt index)
{
	ClassData *me = (ClassData*)this->clsData;
	if (index < 0 || index >= me->sensorCnt)
	{
		return 0;
	}
	switch (ASensor_getType(me->sensorList[index]))
	{
	case ASENSOR_TYPE_ACCELEROMETER:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
#endif
		{
			IO::SensorAccelerometerAndroid *sensor;
			NEW_CLASS(sensor, IO::SensorAccelerometerAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_PRESSURE:
		{
			IO::SensorPressureAndroid *sensor;
			NEW_CLASS(sensor, IO::SensorPressureAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
#endif
	case ASENSOR_TYPE_MAGNETIC_FIELD:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
#endif
		{
			IO::SensorMagnetometerAndroid *sensor;
			NEW_CLASS(sensor, IO::SensorMagnetometerAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
	default:
		{
			IO::SensorUnknownAndroid *sensor;
			printf("Sensor Type = %d\r\n", ASensor_getType(me->sensorList[index]));
			NEW_CLASS(sensor, IO::SensorUnknownAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
	}
}

OSInt IO::SensorManager::GetAccelerometerCnt()
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0)
		return 0;

	OSInt ret = 0;
	OSInt i = this->GetSensorCnt();
	while (i-- > 0)
	{
		if (this->GetSensorType(i) == IO::Sensor::ST_ACCELEROMETER)
		{
			ret++;
		}
	}
	return ret;
}

IO::SensorAccelerometer *IO::SensorManager::CreateAccelerometer(OSInt index)
{
	IO::SensorAccelerometer *ret = 0;
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0 || index < 0)
		return 0;

	OSInt i = 0;
	OSInt j = this->GetSensorCnt();
	while (i < j)
	{
		if (this->GetSensorType(i) == IO::Sensor::ST_ACCELEROMETER)
		{
			if (index == 0)
			{
				ret = this->CreateSensor(i)->GetSensorAccelerator();
				break;
			}
			else
			{
				index--;
			}
			
		}
		i++;
	}
	return ret;
}
