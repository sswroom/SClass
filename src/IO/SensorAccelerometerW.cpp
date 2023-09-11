#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAccelerometerW.h"

#include <windows.h>
#include <sensorsapi.h>
#include <sensors.h>
/*#include <initguid.h>
#include <Sensors.h>*/

IO::SensorAccelerometerW::SensorAccelerometerW(void *sensor) : IO::SensorWin(sensor)
{
	this->clsData = 0;
}

IO::SensorAccelerometerW::~SensorAccelerometerW()
{
}

Bool IO::SensorAccelerometerW::ReadAcceleration(OutParam<Math::Vector3> acc)
{
	Double retX = 0;
	Double retY = 0;
	Double retZ = 0;
	ISensorDataReport *pReport;
	HRESULT hr;
	ISensor *pSensor = (ISensor*)this->sensor;
	hr = pSensor->GetData(&pReport);
	if (SUCCEEDED(hr))
	{
		PROPVARIANT var = {};
		Bool ret = true;
		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_X_G, &var)))
		{
			if (var.vt == VT_R8)
			{
				retX = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				retX = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Y_G, &var)))
		{
			if (var.vt == VT_R8)
			{
				retY = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				retY = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Z_G, &var)))
		{
			if (var.vt == VT_R8)
			{
				retZ = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				retZ = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (ret)
		{
			acc.Set(Math::Vector3(retX, retY, retZ));
		}
		pReport->Release();
		return ret;
	}
	else
	{
		return false;
	}
}

Math::Unit::Acceleration::AccelerationUnit IO::SensorAccelerometerW::GetAccelerationUnit()
{
	return Math::Unit::Acceleration::AccelerationUnit::StandardGravity;
}

IO::SensorAccelerometer *IO::SensorAccelerometerW::GetSensorAccelerator()
{
	return this;
}

IO::Sensor::SensorType IO::SensorAccelerometerW::GetSensorType()
{
	return IO::Sensor::SensorType::Accelerometer;
}
