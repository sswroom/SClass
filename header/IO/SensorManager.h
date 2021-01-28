#ifndef _SM_IO_SENSORMANAGER
#define _SM_IO_SENSORMANAGER
#include "IO/SensorAccelerometer.h"
#include "IO/Sensor.h"

namespace IO
{
	class SensorManager
	{
	private:
		void *clsData;

	public:
		SensorManager();
		~SensorManager();

		OSInt GetSensorCnt();
		IO::Sensor::SensorType GetSensorType(OSInt index);
		Sensor *CreateSensor(OSInt index);

		OSInt GetAccelerometerCnt();
		SensorAccelerometer *CreateAccelerometer(OSInt index);
	};
};
#endif
