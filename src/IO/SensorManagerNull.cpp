#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorManager.h"

IO::SensorManager::SensorManager()
{
	this->clsData = 0;
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

Optional<IO::Sensor> IO::SensorManager::CreateSensor(UOSInt index)
{
	return nullptr;
}

UOSInt IO::SensorManager::GetAccelerometerCnt()
{
	return 0;
}

Optional<IO::SensorAccelerometer> IO::SensorManager::CreateAccelerometer(UOSInt index)
{
	return nullptr;
}
