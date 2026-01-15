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

		UIntOS GetSensorCnt();
		IO::Sensor::SensorType GetSensorType(UIntOS index);
		Optional<Sensor> CreateSensor(UIntOS index);

		UIntOS GetAccelerometerCnt();
		Optional<SensorAccelerometer> CreateAccelerometer(UIntOS index);
	};
}
#endif
