#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorUnknownAndroid.h"

IO::SensorUnknownAndroid::SensorUnknownAndroid(void *sensor, void *sensorMgr, Int32 id) : IO::SensorAndroid(sensor, sensorMgr, id)
{

}

IO::SensorUnknownAndroid::~SensorUnknownAndroid()
{

}

IO::Sensor::SensorType IO::SensorUnknownAndroid::GetSensorType()
{
	return IO::Sensor::SensorType::Unknown;
}
