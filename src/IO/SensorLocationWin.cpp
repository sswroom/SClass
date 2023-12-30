#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorLocationWin.h"

#include <windows.h>
#include <sensorsapi.h>
#include <sensors.h>

IO::SensorLocationWin::SensorLocationWin(void *sensor) : IO::SensorWin(sensor)
{
}

IO::SensorLocationWin::~SensorLocationWin()
{
}

Bool IO::SensorLocationWin::ReadLocation(NotNullPtr<Map::GPSTrack::GPSRecord3> loc)
{
	ISensorDataReport *pReport;
	HRESULT hr;
	ISensor *pSensor = (ISensor*)this->sensor;
	hr = pSensor->GetData(&pReport);
	if (SUCCEEDED(hr))
	{
		PROPVARIANT var = {};
		Bool ret = true;

		loc->recTime = Data::TimeInstant::Now();
		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_LATITUDE_DEGREES, &var)) && var.vt == VT_R8)
		{
			loc->pos.SetLat(var.dblVal);
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_LONGITUDE_DEGREES, &var)) && var.vt == VT_R8)
		{
			loc->altitude = var.dblVal;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_ALTITUDE_SEALEVEL_METERS, &var)) && var.vt == VT_R8)
		{
			loc->pos.SetLon(var.dblVal);
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_SPEED_KNOTS, &var)) && var.vt == VT_R8)
		{
			loc->speed = var.dblVal;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_TRUE_HEADING_DEGREES, &var)) && var.vt == VT_R8)
		{
			loc->heading = var.dblVal;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_GPS_STATUS, &var)) && var.vt == VT_I4)
		{
			loc->valid = (var.intVal == 1);
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_SATELLITES_USED_COUNT, &var)) && var.vt == VT_I4)
		{
			loc->nSateUsed = (UInt8)var.intVal;
			loc->nSateUsedGPS = loc->nSateUsed;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		loc->nSateUsedSBAS = 0;
		loc->nSateUsedGLO = 0;
		if (SUCCEEDED(pReport->GetSensorValue(SENSOR_DATA_TYPE_SATELLITES_IN_VIEW, &var)) && var.vt == VT_I4)
		{
			loc->nSateViewGPS = (UInt8)var.intVal;
		}
		else
		{
			ret = false;
		}
		PropVariantClear(&var);

		loc->nSateViewGLO = 0;
		loc->nSateViewGA = 0;
		loc->nSateViewQZSS = 0;
		loc->nSateViewBD = 0;

		pReport->Release();
		return ret;
	}
	else
	{
		return false;
	}
}

Optional<IO::SensorLocation> IO::SensorLocationWin::GetSensorLocation()
{
	return *this;
}

IO::Sensor::SensorType IO::SensorLocationWin::GetSensorType()
{
	return IO::Sensor::SensorType::Location;
}
