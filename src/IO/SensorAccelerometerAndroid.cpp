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

Bool IO::SensorAccelerometerAndroid::ReadAcceleration(Double *x, Double *y, Double *z)
{
	ASensorEvent sensorEvt;
	if (!this->GetSensorEvent(&sensorEvt))
		return false;
	*x = sensorEvt.acceleration.x;
	*y = sensorEvt.acceleration.y;
	*z = sensorEvt.acceleration.z;
	return true;
}

Math::Unit::Acceleration::AccelerationUnit IO::SensorAccelerometerAndroid::GetAccelerationUnit()
{
	return Math::Unit::Acceleration::AU_METER_PER_SECOND_SQUARED;
}

IO::SensorAccelerometer *IO::SensorAccelerometerAndroid::GetSensorAccelerator()
{
	return this;
}

IO::Sensor::SensorType IO::SensorAccelerometerAndroid::GetSensorType()
{
	return IO::Sensor::ST_ACCELEROMETER;
}