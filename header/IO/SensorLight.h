#ifndef _SM_IO_SENSORLIGHT
#define _SM_IO_SENSORLIGHT

namespace IO
{
	class SensorLight
	{
	public:
		virtual ~SensorLight() {};

		virtual Bool ReadLights(OutParam<Double> lux, OutParam<Double> colorTemp) = 0;
	};
}
#endif
