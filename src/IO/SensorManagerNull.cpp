#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorManager.h"

IO::SensorManager::SensorManager()
{
}

IO::SensorManager::~SensorManager()
{
}

OSInt IO::SensorManager::GetSensorCnt()
{
	return 0;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(OSInt index)
{
	return IO::Sensor::ST_UNKNOWN;
}

IO::Sensor *IO::SensorManager::CreateSensor(OSInt index)
{
	return 0;
}

OSInt IO::SensorManager::GetAccelerometerCnt()
{
	return 0;
}

IO::SensorAccelerometer *IO::SensorManager::CreateAccelerometer(OSInt index)
{
	return 0;
}
