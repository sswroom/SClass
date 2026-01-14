#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Sensor.h"

IO::Sensor::~Sensor()
{
}

Optional<IO::SensorAccelerometer> IO::Sensor::GetSensorAccelerator()
{
	return nullptr;
}

Optional<IO::SensorPressure> IO::Sensor::GetSensorPressure()
{
	return nullptr;
}

Optional<IO::SensorMagnetometer> IO::Sensor::GetSensorMagnetometer()
{
	return nullptr;
}

Optional<IO::SensorOrientation> IO::Sensor::GetSensorOrientation()
{
	return nullptr;
}

Optional<IO::SensorLocation> IO::Sensor::GetSensorLocation()
{
	return nullptr;
}

Optional<IO::SensorLight> IO::Sensor::GetSensorLight()
{
	return nullptr;
}

Text::CStringNN IO::Sensor::GetSensorTypeName(SensorType sensorType)
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
	case SensorType::Location:
		return CSTR("Location");
	case SensorType::Light:
		return CSTR("Light");
	case SensorType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
