#ifndef _SM_IO_SENSORLIGHT
#define _SM_IO_SENSORLIGHT

namespace IO
{
	class SensorLight
	{
	public:
		virtual ~SensorLight() {};

		virtual Bool ReadLights(Double *lux, Double *colorTemp) = 0;
	};
}
#endif
