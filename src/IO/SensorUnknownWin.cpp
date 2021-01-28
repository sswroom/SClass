#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SensorUnknownWin.h"

IO::SensorUnknownWin::SensorUnknownWin(void *sensor) : IO::SensorWin(sensor)
{

}

IO::SensorUnknownWin::~SensorUnknownWin()
{

}

IO::Sensor::SensorType IO::SensorUnknownWin::GetSensorType()
{
	return IO::Sensor::ST_UNKNOWN;
}
