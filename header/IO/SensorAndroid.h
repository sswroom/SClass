#ifndef _SM_IO_SENSORANDROID
#define _SM_IO_SENSORANDROID
#include "IO/Sensor.h"

namespace IO
{
	class SensorAndroid : public IO::Sensor
	{
	protected:
		void *sensor;
		void *sensorMgr;
		Int32 id;
		void *queue;

	public:
		SensorAndroid(void *sensor, void *sensorMgr, Int32 id);
		virtual ~SensorAndroid();

		virtual Text::CString GetVendor();
		virtual Text::CString GetName();
		virtual Bool EnableSensor();
		virtual Bool DisableSensor();

		Bool GetSensorEvent(void *sensorEvt);
	};
}

#endif
