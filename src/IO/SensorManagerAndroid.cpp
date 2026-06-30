#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAccelerometerAndroid.h"
#include "IO/SensorManager.h"
#include "IO/SensorMagnetometerAndroid.h"
#include "IO/SensorPressureAndroid.h"
#include "IO/SensorUnknownAndroid.h"

#include <stdio.h>
#include <android/sensor.h>

struct IO::SensorManager::ClassData
{
	ASensorManager *mgr;
	ASensorList sensorList;
	IntOS sensorCnt;
};

IO::SensorManager::SensorManager()
{
	NN<ClassData> me = MemAllocNN(ClassData);
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
	MemFreeNN(this->clsData);
}

UIntOS IO::SensorManager::GetSensorCnt()
{
	return this->clsData->sensorCnt;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(UIntOS index)
{
	NN<ClassData> me = this->clsData;
	if (index >= me->sensorCnt)
	{
		return IO::Sensor::SensorType::Unknown;
	}
	switch (ASensor_getType(me->sensorList[index]))
	{
	case ASENSOR_TYPE_ACCELEROMETER:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
#endif
		return IO::Sensor::SensorType::Accelerometer;
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_PRESSURE:
		return IO::Sensor::SensorType::Pressure;
#endif
	case ASENSOR_TYPE_MAGNETIC_FIELD:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
#endif
		return IO::Sensor::SensorType::Magnetometer;
	default:
		return IO::Sensor::SensorType::Unknown;
	}

}

Optional<IO::Sensor> IO::SensorManager::CreateSensor(UIntOS index)
{
	NN<ClassData> me = this->clsData;
	if (index < 0 || index >= me->sensorCnt)
	{
		return nullptr;
	}
	switch (ASensor_getType(me->sensorList[index]))
	{
	case ASENSOR_TYPE_ACCELEROMETER:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
#endif
		{
			NN<IO::SensorAccelerometerAndroid> sensor;
			NEW_CLASSNN(sensor, IO::SensorAccelerometerAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_PRESSURE:
		{
			NN<IO::SensorPressureAndroid> sensor;
			NEW_CLASSNN(sensor, IO::SensorPressureAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
#endif
	case ASENSOR_TYPE_MAGNETIC_FIELD:
#if __ANDROID_API__ > 16
	case ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
#endif
		{
			NN<IO::SensorMagnetometerAndroid> sensor;
			NEW_CLASSNN(sensor, IO::SensorMagnetometerAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
	default:
		{
			NN<IO::SensorUnknownAndroid> sensor;
			printf("Sensor Type = %d\r\n", ASensor_getType(me->sensorList[index]));
			NEW_CLASSNN(sensor, IO::SensorUnknownAndroid((void*)me->sensorList[index], me->mgr, (Int32)index + 1));
			return sensor;
		}
	}
}

UIntOS IO::SensorManager::GetAccelerometerCnt()
{
	NN<ClassData> me = this->clsData;
	if (me->mgr == 0)
		return 0;

	IntOS ret = 0;
	IntOS i = this->GetSensorCnt();
	while (i-- > 0)
	{
		if (this->GetSensorType(i) == IO::Sensor::SensorType::Accelerometer)
		{
			ret++;
		}
	}
	return ret;
}

Optional<IO::SensorAccelerometer> IO::SensorManager::CreateAccelerometer(UIntOS index)
{
	Optional<IO::SensorAccelerometer> ret = nullptr;	
	NN<ClassData> me = this->clsData;
	if (me->mgr == 0 || index < 0)
		return nullptr;

	IntOS i = 0;
	IntOS j = this->GetSensorCnt();
	while (i < j)
	{
		if (this->GetSensorType(i) == IO::Sensor::SensorType::Accelerometer)
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
