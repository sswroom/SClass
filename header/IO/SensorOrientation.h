#ifndef _SM_IO_SENSORORIENTATION
#define _SM_IO_SENSORORIENTATION
//#include "Math/Unit/Pressure.h"

namespace IO
{
	class SensorOrientation
	{
	public:
		virtual ~SensorOrientation() {};

		virtual Bool ReadOrientation(Double *x, Double *y, Double *z) = 0;
//		virtual Math::Unit::Pressure::PressureUnit GetPressureUnit() = 0;
	};
};

#endif
