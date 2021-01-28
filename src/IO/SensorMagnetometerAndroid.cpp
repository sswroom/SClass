#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorMagnetometerAndroid.h"
#include <android/sensor.h>

IO::SensorMagnetometerAndroid::SensorMagnetometerAndroid(void *sensor, void *sensorMgr, Int32 id) : IO::SensorAndroid(sensor, sensorMgr, id)
{

}

IO::SensorMagnetometerAndroid::~SensorMagnetometerAndroid()
{

}

Bool IO::SensorMagnetometerAndroid::ReadMagneticField(Double *x, Double *y, Double *z)
{
	ASensorEvent sensorEvt;
	if (!this->GetSensorEvent(&sensorEvt))
		return false;
	*x = sensorEvt.magnetic.x;
	*y = sensorEvt.magnetic.y;
	*z = sensorEvt.magnetic.z;
	return true;
}

Math::Unit::MagneticField::MagneticFieldUnit IO::SensorMagnetometerAndroid::GetMagneticFieldUnit()
{
	return Math::Unit::MagneticField::MFU_MICROTESLA;
}

IO::SensorMagnetometer *IO::SensorMagnetometerAndroid::GetSensorMagnetometer()
{
	return this;
}

IO::Sensor::SensorType IO::SensorMagnetometerAndroid::GetSensorType()
{
	return IO::Sensor::ST_MAGNETOMETER;
}