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

		UOSInt GetSensorCnt();
		IO::Sensor::SensorType GetSensorType(UOSInt index);
		Optional<Sensor> CreateSensor(UOSInt index);

		UOSInt GetAccelerometerCnt();
		Optional<SensorAccelerometer> CreateAccelerometer(UOSInt index);
	};
}
#endif
