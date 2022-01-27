#ifndef _SM_IO_SENSOR
#define _SM_IO_SENSOR
#include "IO/SensorAccelerometer.h"
#include "IO/SensorOrientation.h"
#include "IO/SensorPressure.h"
#include "IO/SensorMagnetometer.h"
#include "Text/CString.h"

namespace IO
{
	class Sensor
	{
	public:
		enum class SensorType
		{
			Unknown,
			Accelerometer,
			Pressure,
			Magnetometer,
			Orientation
		};
	public:
		virtual ~Sensor();

		virtual const UTF8Char *GetVendor() = 0;
		virtual const UTF8Char *GetName() = 0;
		virtual SensorType GetSensorType() = 0;
		virtual Bool EnableSensor() = 0;
		virtual Bool DisableSensor() = 0;
		virtual IO::SensorAccelerometer *GetSensorAccelerator();
		virtual IO::SensorPressure *GetSensorPressure();
		virtual IO::SensorMagnetometer *GetSensorMagnetometer();
		virtual IO::SensorOrientation *GetSensorOrientation();

		static Text::CString GetSensorTypeName(SensorType sensorType);
	};
}

#endif
