#ifndef _SM_IO_SENSORLIGHTWIN
#define _SM_IO_SENSORLIGHTWIN
#include "IO/SensorWin.h"
#include "IO/SensorLight.h"

namespace IO
{
	class SensorLightWin : public SensorWin, public SensorLight
	{
	public:
		SensorLightWin(void *sensor);
		virtual ~SensorLightWin();

		virtual Bool ReadLights(Double *lux, Double *colorTemp);
		virtual SensorLight *GetSensorLight();
		virtual IO::Sensor::SensorType GetSensorType();
	};
}
#endif
