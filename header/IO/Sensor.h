#ifndef _SM_IO_SENSOR
#define _SM_IO_SENSOR
#include "IO/SensorAccelerometer.h"
#include "IO/SensorLight.h"
#include "IO/SensorLocation.h"
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
			Orientation,
			Location,
			Light
		};
	public:
		virtual ~Sensor();

		virtual Text::CString GetVendor() = 0;
		virtual Text::CString GetName() = 0;
		virtual SensorType GetSensorType() = 0;
		virtual Bool EnableSensor() = 0;
		virtual Bool DisableSensor() = 0;
		virtual Optional<IO::SensorAccelerometer> GetSensorAccelerator();
		virtual Optional<IO::SensorLight> GetSensorLight();
		virtual Optional<IO::SensorPressure> GetSensorPressure();
		virtual Optional<IO::SensorMagnetometer> GetSensorMagnetometer();
		virtual Optional<IO::SensorOrientation> GetSensorOrientation();
		virtual Optional<IO::SensorLocation> GetSensorLocation();

		static Text::CStringNN GetSensorTypeName(SensorType sensorType);
	};
}

#endif
