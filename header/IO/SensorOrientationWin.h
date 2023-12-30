#ifndef _SM_IO_SENSORORIENTATIONWIN
#define _SM_IO_SENSORORIENTATIONWIN
#include "IO/SensorWin.h"
#include "IO/SensorOrientation.h"

namespace IO
{
	class SensorOrientationWin : public SensorWin, public SensorOrientation
	{
	public:
		SensorOrientationWin(void *sensor);
		virtual ~SensorOrientationWin();

		virtual Bool ReadOrientation(OutParam<Double> x, OutParam<Double> y, OutParam<Double> z);
//		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit();
		virtual Optional<SensorOrientation> GetSensorOrientation();
		virtual IO::Sensor::SensorType GetSensorType();
	};
};

#endif
