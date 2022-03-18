#ifndef _SM_IO_SENSORWIN
#define _SM_IO_SENSORWIN
#include "IO/Sensor.h"
#include "Text/String.h"

namespace IO
{
	class SensorWin : public IO::Sensor
	{
	protected:
		void *sensor;
		Text::String *name;

	public:
		SensorWin(void *sensor);
		virtual ~SensorWin();

		virtual Text::CString GetVendor();
		virtual Text::CString GetName();
		virtual Bool EnableSensor();
		virtual Bool DisableSensor();
	};
};

#endif
