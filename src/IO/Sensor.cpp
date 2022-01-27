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

Text::CString IO::Sensor::GetSensorTypeName(SensorType sensorType)
{
	switch (sensorType)
	{
	case SensorType::Accelerometer:
		return CSTR("Accelerometer");
	case SensorType::Pressure:
		return CSTR("Pressure");
	case SensorType::Magnetometer:
		return CSTR("Magnetometer");
	case SensorType::Orientation:
		return CSTR("Orientation");
	case SensorType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
