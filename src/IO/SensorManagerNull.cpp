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

UIntOS IO::SensorManager::GetSensorCnt()
{
	return 0;
}

IO::Sensor::SensorType IO::SensorManager::GetSensorType(UIntOS index)
{
	return IO::Sensor::SensorType::Unknown;
}

Optional<IO::Sensor> IO::SensorManager::CreateSensor(UIntOS index)
{
	return nullptr;
}

UIntOS IO::SensorManager::GetAccelerometerCnt()
{
	return 0;
}

Optional<IO::SensorAccelerometer> IO::SensorManager::CreateAccelerometer(UIntOS index)
{
	return nullptr;
}
