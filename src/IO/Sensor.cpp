#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Sensor.h"

IO::Sensor::~Sensor()
{
}

IO::SensorAccelerometer *IO::Sensor::GetSensorAccelerator()
{
	return 0;
}

IO::SensorPressure *IO::Sensor::GetSensorPressure()
{
	return 0;
}

IO::SensorMagnetometer *IO::Sensor::GetSensorMagnetometer()
{
	return 0;
}

IO::SensorOrientation *IO::Sensor::GetSensorOrientation()
{
	return 0;
}

const UTF8Char *IO::Sensor::GetSensorTypeName(SensorType sensorType)
{
	switch (sensorType)
	{
	case ST_ACCELEROMETER:
		return (const UTF8Char*)"Accelerometer";
	case ST_PRESSURE:
		return (const UTF8Char*)"Pressure";
	case ST_MAGNETOMETER:
		return (const UTF8Char*)"Magnetometer";
	case ST_ORIENTATION:
		return (const UTF8Char*)"Orientation";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
