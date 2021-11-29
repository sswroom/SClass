#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorOrientationWin.h"

#include <windows.h>
#include <sensorsapi.h>
#include <sensors.h>
/*#include <initguid.h>
#include <Sensors.h>*/

IO::SensorOrientationWin::SensorOrientationWin(void *sensor) : IO::SensorWin(sensor)
{
}

IO::SensorOrientationWin::~SensorOrientationWin()
{
}

Bool IO::SensorOrientationWin::ReadOrientation(Double *x, Double *y, Double *z)
{
	ISensorDataReport *pReport;
	HRESULT hr;
	ISensor *pSensor = (ISensor*)this->sensor;
	hr = pSensor->GetData(&pReport);
	if (SUCCEEDED(hr))
	{
		PROPVARIANT var = {};
		Bool ret = true;
		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_TILT_X_DEGREES, &var)))
		{
			if (var.vt == VT_R8)
			{
				*x = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				*x = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_TILT_Y_DEGREES, &var)))
		{
			if (var.vt == VT_R8)
			{
				*y = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				*y = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_TILT_Z_DEGREES, &var)))
		{
			if (var.vt == VT_R8)
			{
				*z = var.dblVal;
			}
			else if (var.vt == VT_R4)
			{
				*z = var.fltVal;
			}
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		pReport->Release();
		return ret;
	}
	else
	{
		return false;
	}
}

/*Math::Unit::Acceleration::AccelerationUnit IO::SensorAccelerometerW::GetAccelerationUnit()
{
	return Math::Unit::Acceleration::AU_STANDARD_GRAVITY;
}*/

IO::SensorOrientation *IO::SensorOrientationWin::GetSensorOrientation()
{
	return this;
}

IO::Sensor::SensorType IO::SensorOrientationWin::GetSensorType()
{
	return IO::Sensor::SensorType::Orientation;
}
