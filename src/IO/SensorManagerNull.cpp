#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorManager.h"

IO::SensorManager::SensorManager()
{
}

IO::SensorManager::~SensorManager()
{
}

UOSInt IO::SensorManager::GetSensorCnt()
{
	return 0;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(UOSInt index)
{
	return IO::Sensor::SensorType::Unknown;
}

IO::Sensor *IO::SensorManager::CreateSensor(UOSInt index)
{
	return 0;
}

UOSInt IO::SensorManager::GetAccelerometerCnt()
{
	return 0;
}

IO::SensorAccelerometer *IO::SensorManager::CreateAccelerometer(UOSInt index)
{
	return 0;
}
