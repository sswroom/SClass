#ifndef _SM_IO_SENSORMAGNETOMETER
#define _SM_IO_SENSORMAGNETOMETER
#include "Math/Vector3.h"
#include "Math/Unit/MagneticField.h"

namespace IO
{
	class SensorMagnetometer
	{
	public:
		virtual ~SensorMagnetometer() {};

		virtual Bool ReadMagneticField(OutParam<Math::Vector3> mag) = 0;
		virtual Math::Unit::MagneticField::MagneticFieldUnit GetMagneticFieldUnit() = 0;
	};
}
#endif
