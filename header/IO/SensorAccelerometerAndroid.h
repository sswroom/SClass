#ifndef _SM_IO_SENSORACCELEROMETERANDROID
#define _SM_IO_SENSORACCELEROMETERANDROID
#include "IO/SensorAccelerometer.h"
#include "IO/SensorAndroid.h"

namespace IO
{
	class SensorAccelerometerAndroid : public IO::SensorAndroid, public SensorAccelerometer
	{
	public:
		SensorAccelerometerAndroid(void *sensor, void *sensorMgr, Int32 id);
		virtual ~SensorAccelerometerAndroid();

		virtual Bool ReadAcceleration(Double *x, Double *y, Double *z);
		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit();
		virtual SensorAccelerometer *GetSensorAccelerator();
		virtual SensorType GetSensorType();
	};
};

#endif
