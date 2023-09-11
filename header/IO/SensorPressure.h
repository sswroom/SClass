#ifndef _SM_IO_SENSORPRESSURE
#define _SM_IO_SENSORPRESSURE
#include "Math/Unit/Pressure.h"

namespace IO
{
	class SensorPressure
	{
	public:
		virtual ~SensorPressure() {};

		virtual Bool ReadPressure(OutParam<Double> pressure) = 0;
		virtual Math::Unit::Pressure::PressureUnit GetPressureUnit() = 0;
	};
};

#endif
