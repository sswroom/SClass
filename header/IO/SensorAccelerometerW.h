#ifndef _SM_IO_SENSORACCELEROMETERW
#define _SM_IO_SENSORACCELEROMETERW
#include "IO/SensorWin.h"
#include "IO/SensorAccelerometer.h"

namespace IO
{
	class SensorAccelerometerW : public SensorWin, public SensorAccelerometer
	{
	private:
		void *clsData;
	public:
		SensorAccelerometerW(void *sensor);
		virtual ~SensorAccelerometerW();

		virtual Bool ReadAcceleration(OutParam<Math::Vector3> acc);
		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit();
		virtual SensorAccelerometer *GetSensorAccelerator();
		virtual IO::Sensor::SensorType GetSensorType();
	};
};

#endif
