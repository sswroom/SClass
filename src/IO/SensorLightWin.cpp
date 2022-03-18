#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorLightWin.h"

#include <windows.h>
#include <sensorsapi.h>
#include <sensors.h>

IO::SensorLightWin::SensorLightWin(void *sensor) : IO::SensorWin(sensor)
{
}

IO::SensorLightWin::~SensorLightWin()
{
}

Bool IO::SensorLightWin::ReadLights(Double *lux, Double *colorTemp)
{
	ISensorDataReport *pReport;
	HRESULT hr;
	ISensor *pSensor = (ISensor*)this->sensor;
	hr = pSensor->GetData(&pReport);
	if (SUCCEEDED(hr))
	{
		PROPVARIANT var = {};
		Bool ret = true;

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_LIGHT_LEVEL_LUX, &var)) && var.vt == VT_R4)
		{
			*lux = var.fltVal;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_LIGHT_TEMPERATURE_KELVIN, &var)) && var.vt == VT_R4)
		{
			*colorTemp = var.fltVal;
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

IO::SensorLight *IO::SensorLightWin::GetSensorLight()
{
	return this;
}

IO::Sensor::SensorType IO::SensorLightWin::GetSensorType()
{
	return IO::Sensor::SensorType::Light;
}
