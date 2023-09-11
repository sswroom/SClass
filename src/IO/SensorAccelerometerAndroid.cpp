#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorAccelerometerAndroid.h"
#include <android/sensor.h>

IO::SensorAccelerometerAndroid::SensorAccelerometerAndroid(void *sensor, void *sensorMgr, Int32 id) : IO::SensorAndroid(sensor, sensorMgr, id)
{

}

IO::SensorAccelerometerAndroid::~SensorAccelerometerAndroid()
{

}

Bool IO::SensorAccelerometerAndroid::ReadAcceleration(OutParam<Math::Vector3> acc)
{
	ASensorEvent sensorEvt;
	if (!this->GetSensorEvent(&sensorEvt))
		return false;
	acc.Set(Math::Vector3(sensorEvt.acceleration.x, sensorEvt.acceleration.y, sensorEvt.acceleration.z));
	return true;
}

Math::Unit::Acceleration::AccelerationUnit IO::SensorAccelerometerAndroid::GetAccelerationUnit()
{
	return Math::Unit::Acceleration::AccelerationUnit::MeterPerSecondSquared;
}

IO::SensorAccelerometer *IO::SensorAccelerometerAndroid::GetSensorAccelerator()
{
	return this;
}

IO::Sensor::SensorType IO::SensorAccelerometerAndroid::GetSensorType()
{
	return IO::Sensor::SensorType::Accelerometer;
}