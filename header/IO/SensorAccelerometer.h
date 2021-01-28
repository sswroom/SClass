#ifndef _SM_IO_SENSORACCELEROMETER
#define _SM_IO_SENSORACCELEROMETER
#include "Math/Unit/Acceleration.h"

namespace IO
{
	class SensorAccelerometer
	{
	public:
		virtual ~SensorAccelerometer() {};

		virtual Bool ReadAcceleration(Double *x, Double *y, Double *z) = 0;
		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit() = 0;
	};
};

#endif
