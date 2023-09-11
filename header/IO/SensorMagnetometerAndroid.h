#ifndef _SM_IO_SENSORMAGNETOMETERANDROID
#define _SM_IO_SENSORMAGNETOMETERANDROID
#include "IO/SensorMagnetometer.h"
#include "IO/SensorAndroid.h"

namespace IO
{
	class SensorMagnetometerAndroid : public IO::SensorAndroid, public SensorMagnetometer
	{
	public:
		SensorMagnetometerAndroid(void *sensor, void *sensorMgr, Int32 id);
		virtual ~SensorMagnetometerAndroid();

		virtual Bool ReadMagneticField(OutParam<Math::Vector3> mag);
		virtual Math::Unit::MagneticField::MagneticFieldUnit GetMagneticFieldUnit();
		virtual SensorMagnetometer *GetSensorMagnetometer();
		virtual SensorType GetSensorType();
	};
};

#endif
