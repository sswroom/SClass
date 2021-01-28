#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAccelerometerW.h"
#include "IO/SensorOrientationWin.h"
#include "IO/SensorUnknownWin.h"
#include "IO/SensorManager.h"

#include <windows.h>
#include <initguid.h>
#include <sensorsapi.h>
#include <sensors.h>
#include <stdio.h>

#if !defined(__MINGW32__)
// {77a1c827-fcd2-4689-8915-9d613cc5fa3e}
DEFINE_GUID(CLSID_SensorManager,              0X77a1c827, 0Xfcd2, 0X4689, 0X89, 0X15, 0X9d, 0X61, 0X3c, 0Xc5, 0Xfa, 0X3e);
#endif

typedef struct
{
	ISensorManager *mgr;
	Bool accessDenined;
} ClassData;

IO::SensorManager::SensorManager()
{
	ClassData *me = MemAlloc(ClassData, 1);
	this->clsData = me;
	me->mgr = 0;
	me->accessDenined = false;

	CoInitialize(0);

	ISensorManager *pSensorManager = 0;

	HRESULT hr = CoCreateInstance(CLSID_SensorManager, 
							NULL, CLSCTX_INPROC_SERVER,
							IID_PPV_ARGS(&pSensorManager));

	if(hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DISABLED_BY_POLICY))
	{
		me->accessDenined = true;
	}
	else if (pSensorManager)
	{
		me->mgr = pSensorManager;
	}


}

IO::SensorManager::~SensorManager()
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr)
	{
		me->mgr->Release();
	}
	MemFree(me);
	CoUninitialize();
}

OSInt IO::SensorManager::GetSensorCnt()
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0)
		return 0;

	OSInt ret = 0;
	ISensorCollection *pSensorColl;
	HRESULT hr;
	hr = me->mgr->GetSensorsByCategory(SENSOR_CATEGORY_ALL, &pSensorColl);

	if(SUCCEEDED(hr))
	{
		ULONG ulCount = 0;

		hr = pSensorColl->GetCount(&ulCount);

		if(SUCCEEDED(hr))
		{
			ret = ulCount;
		}
		pSensorColl->Release();
	}
	return ret;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(OSInt index)
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0 || index < 0)
		return IO::Sensor::ST_UNKNOWN;

	IO::Sensor::SensorType sensorType = IO::Sensor::ST_UNKNOWN;
	ISensorCollection *pSensorColl;
	HRESULT hr;
	hr = me->mgr->GetSensorsByCategory(SENSOR_CATEGORY_ALL, &pSensorColl);

	if(SUCCEEDED(hr))
	{
		ULONG ulCount = 0;

		hr = pSensorColl->GetCount(&ulCount);

		if(SUCCEEDED(hr))
		{
			if ((OSInt)ulCount > index)
			{
				ISensor *pSensor;
				if (SUCCEEDED(pSensorColl->GetAt((ULONG)index, &pSensor)))
				{
					SENSOR_TYPE_ID sType;
					if (SUCCEEDED(pSensor->GetType(&sType)))
					{
						if (sType == SENSOR_TYPE_ACCELEROMETER_3D)
						{
							sensorType = IO::Sensor::ST_ACCELEROMETER;
						}
						else if (sType == SENSOR_TYPE_ENVIRONMENTAL_ATMOSPHERIC_PRESSURE)
						{
							sensorType = IO::Sensor::ST_PRESSURE;
						}
						else if (sType == SENSOR_TYPE_COMPASS_3D)
						{
							sensorType = IO::Sensor::ST_MAGNETOMETER;
						}
						else if (sType == SENSOR_TYPE_INCLINOMETER_3D)
						{
							sensorType = IO::Sensor::ST_ORIENTATION;
						}
					}
					pSensor->Release();
				}
			}
		}
		pSensorColl->Release();
	}
	return sensorType;
}

IO::Sensor *IO::SensorManager::CreateSensor(OSInt index)
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0 || index < 0)
		return 0;

	IO::Sensor *ret = 0;
	ISensorCollection *pSensorColl;
	HRESULT hr;
	hr = me->mgr->GetSensorsByCategory(SENSOR_CATEGORY_ALL, &pSensorColl);

	if(SUCCEEDED(hr))
	{
		ULONG ulCount = 0;

		hr = pSensorColl->GetCount(&ulCount);

		if(SUCCEEDED(hr))
		{
			if ((OSInt)ulCount > index)
			{
				ISensor *pSensor;
				if (SUCCEEDED(pSensorColl->GetAt((ULONG)index, &pSensor)))
				{
					SENSOR_TYPE_ID sType;
					if (SUCCEEDED(pSensor->GetType(&sType)))
					{
//						printf("type = %x, %x\r\n", sType.Data1, sType.Data2);
						if (sType == SENSOR_TYPE_ACCELEROMETER_3D)
						{
							NEW_CLASS(ret, IO::SensorAccelerometerW(pSensor));
						}
						else if (sType == SENSOR_TYPE_ENVIRONMENTAL_ATMOSPHERIC_PRESSURE)
						{
							NEW_CLASS(ret, IO::SensorUnknownWin(pSensor));
						}
						else if (sType == SENSOR_TYPE_COMPASS_3D)
						{
							NEW_CLASS(ret, IO::SensorUnknownWin(pSensor));
						}
						else if (sType == SENSOR_TYPE_INCLINOMETER_3D)
						{
							NEW_CLASS(ret, IO::SensorOrientationWin(pSensor));
						}
						else
						{
							NEW_CLASS(ret, IO::SensorUnknownWin(pSensor));
						}
					}
					else
					{
						NEW_CLASS(ret, IO::SensorUnknownWin(pSensor));
					}
				}
			}
		}
		pSensorColl->Release();
	}
	return ret;
}

OSInt IO::SensorManager::GetAccelerometerCnt()
{
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0)
		return 0;

	OSInt ret = 0;
	ISensorCollection *pSensorColl;
	HRESULT hr;
	hr = me->mgr->GetSensorsByType(SENSOR_TYPE_ACCELEROMETER_3D, &pSensorColl);

	if(SUCCEEDED(hr))
	{
		ULONG ulCount = 0;

		hr = pSensorColl->GetCount(&ulCount);

		if(SUCCEEDED(hr))
		{
			ret = ulCount;
		}
		pSensorColl->Release();
	}
	return ret;
}

IO::SensorAccelerometer *IO::SensorManager::CreateAccelerometer(OSInt index)
{
	IO::SensorAccelerometer *ret = 0;
	ClassData *me = (ClassData*)this->clsData;
	if (me->mgr == 0 || index < 0)
		return 0;

	ISensorCollection *pSensorColl;
	HRESULT hr;
	hr = me->mgr->GetSensorsByType(SENSOR_TYPE_ACCELEROMETER_3D, &pSensorColl);

	if(SUCCEEDED(hr))
	{
		ULONG ulCount = 0;

		hr = pSensorColl->GetCount(&ulCount);

		if(SUCCEEDED(hr))
		{
			if ((OSInt)ulCount > index)
			{
				ISensor *pSensor;
				if (SUCCEEDED(pSensorColl->GetAt((ULONG)index, &pSensor)))
				{
					NEW_CLASS(ret, IO::SensorAccelerometerW(pSensor));
				}
			}
		}
		pSensorColl->Release();
	}
	return ret;
}
