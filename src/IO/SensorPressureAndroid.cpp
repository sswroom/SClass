#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorPressureAndroid.h"
#include <android/sensor.h>

IO::SensorPressureAndroid::SensorPressureAndroid(void *sensor, void *sensorMgr, Int32 id) : IO::SensorAndroid(sensor, sensorMgr, id)
{

}

IO::SensorPressureAndroid::~SensorPressureAndroid()
{

}

Bool IO::SensorPressureAndroid::ReadPressure(Double *pressure)
{
	ASensorEvent sensorEvt;
	if (!this->GetSensorEvent(&sensorEvt))
		return false;
	*pressure = sensorEvt.pressure;
	return true;
}

Math::Unit::Pressure::PressureUnit IO::SensorPressureAndroid::GetPressureUnit()
{
	return Math::Unit::Pressure::PU_HPASCAL;
}

IO::SensorPressure *IO::SensorPressureAndroid::GetSensorPressure()
{
	return this;
}

IO::Sensor::SensorType IO::SensorPressureAndroid::GetSensorType()
{
	return IO::Sensor::ST_PRESSURE;
}