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

		virtual const UTF8Char *GetVendor();
		virtual const UTF8Char *GetName();
		virtual Bool EnableSensor();
		virtual Bool DisableSensor();

		Bool GetSensorEvent(void *sensorEvt);
	};
};

#endif
