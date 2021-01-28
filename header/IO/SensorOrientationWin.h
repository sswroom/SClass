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

		virtual Bool ReadOrientation(Double *x, Double *y, Double *z);
//		virtual Math::Unit::Acceleration::AccelerationUnit GetAccelerationUnit();
		virtual SensorOrientation *GetSensorOrientation();
		virtual IO::Sensor::SensorType GetSensorType();
	};
};

#endif
