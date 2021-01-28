#ifndef _SM_IO_SENSORUNKNOWNANDROID
#define _SM_IO_SENSORUNKNOWNANDROID
#include "IO/SensorAndroid.h"

namespace IO
{
	class SensorUnknownAndroid : public IO::SensorAndroid
	{
	public:
		SensorUnknownAndroid(void *sensor, void *sensorMgr, Int32 id);
		virtual ~SensorUnknownAndroid();

		virtual SensorType GetSensorType();
	};
};

#endif
