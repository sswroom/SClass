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

Bool IO::SensorMagnetometerAndroid::ReadMagneticField(OutParam<Math::Vector3> mag)
{
	ASensorEvent sensorEvt;
	if (!this->GetSensorEvent(&sensorEvt))
		return false;
	mag.Set(Math::Vector3(sensorEvt.magnetic.x, sensorEvt.magnetic.y, sensorEvt.magnetic.z));
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
	return IO::Sensor::SensorType::Magnetometer;
}