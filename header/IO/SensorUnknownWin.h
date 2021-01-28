#ifndef _SM_IO_SENSORUNKNOWNWIN
#define _SM_IO_SENSORUNKNOWNWIN
#include "IO/SensorWin.h"

namespace IO
{
	class SensorUnknownWin : public IO::SensorWin
	{
	public:
		SensorUnknownWin(void *sensor);
		virtual ~SensorUnknownWin();

		virtual SensorType GetSensorType();
	};
};

#endif
