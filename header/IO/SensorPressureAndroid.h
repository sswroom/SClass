#ifndef _SM_IO_SENSORPRESSUREANDROID
#define _SM_IO_SENSORPRESSUREANDROID
#include "IO/SensorAndroid.h"
#include "IO/SensorPressure.h"

namespace IO
{
	class SensorPressureAndroid : public IO::SensorAndroid, public SensorPressure
	{
	public:
		SensorPressureAndroid(void *sensor, void *sensorMgr, Int32 id);
		virtual ~SensorPressureAndroid();

		virtual Bool ReadPressure(OutParam<Double> pressure);
		virtual Math::Unit::Pressure::PressureUnit GetPressureUnit();
		virtual SensorPressure *GetSensorPressure();
		virtual SensorType GetSensorType();
	};
};

#endif
