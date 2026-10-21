#ifndef _SM_IO_SENSORMAGNETOMETER
#define _SM_IO_SENSORMAGNETOMETER
#include "Math/Vector3D.h"
#include "Math/Unit/MagneticField.h"

namespace IO
{
	class SensorMagnetometer
	{
	public:
		virtual ~SensorMagnetometer() {};

		virtual Bool ReadMagneticField(OutParam<Math::Vector3D> mag) = 0;
		virtual Math::Unit::MagneticField::MagneticFieldUnit GetMagneticFieldUnit() = 0;
	};
}
#endif
