#ifndef _SM_IO_SENSORMAGNETOMETER
#define _SM_IO_SENSORMAGNETOMETER
#include "Math/Unit/MagneticField.h"

namespace IO
{
	class SensorMagnetometer
	{
	public:
		virtual ~SensorMagnetometer() {};

		virtual Bool ReadMagneticField(Double *x, Double *y, Double *z) = 0;
		virtual Math::Unit::MagneticField::MagneticFieldUnit GetMagneticFieldUnit() = 0;
	};
};

#endif
