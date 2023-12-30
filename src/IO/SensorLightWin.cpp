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

Bool IO::SensorLightWin::ReadLights(OutParam<Double> lux, OutParam<Double> colorTemp)
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
			lux.Set(var.fltVal);
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_LIGHT_TEMPERATURE_KELVIN, &var)) && var.vt == VT_R4)
		{
			colorTemp.Set(var.fltVal);
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

Optional<IO::SensorLight> IO::SensorLightWin::GetSensorLight()
{
	return *this;
}

IO::Sensor::SensorType IO::SensorLightWin::GetSensorType()
{
	return IO::Sensor::SensorType::Light;
}
