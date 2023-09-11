#ifndef _SM_IO_SENSORACCELEROMETER
#define _SM_IO_SENSORACCELEROMETER
#include "Math/Vector3.h"
#include "Math/Unit/Acceleration.h"

namespace IO
{
	class SensorAccelerometer
	{
	public:
		virtual ~SensorAccelerometer() {};

		virtual Bool ReadAcceleration(OutParam<Math::Vector3> acc) = 0;
		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit() = 0;
	};
};

#endif
