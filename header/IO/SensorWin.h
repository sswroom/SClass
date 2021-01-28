#ifndef _SM_IO_SENSORWIN
#define _SM_IO_SENSORWIN
#include "IO/Sensor.h"

namespace IO
{
	class SensorWin : public IO::Sensor
	{
	protected:
		void *sensor;
		const UTF8Char *name;

	public:
		SensorWin(void *sensor);
		virtual ~SensorWin();

		virtual const UTF8Char *GetVendor();
		virtual const UTF8Char *GetName();
		virtual Bool EnableSensor();
		virtual Bool DisableSensor();
	};
};

#endif
