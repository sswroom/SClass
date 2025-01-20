#ifndef _SM_IO_GPUCONTROL
#define _SM_IO_GPUCONTROL
#include "Text/String.h"

namespace IO
{
	class GPUControl
	{
	public:
		virtual ~GPUControl() {}

		virtual NN<Text::String> GetName() = 0;
		virtual Bool GetTemperature(OutParam<Double> temp) = 0;
		virtual Bool GetCoreClock(OutParam<Double> mhz) = 0;
		virtual Bool GetMemoryClock(OutParam<Double> mhz) = 0;
		virtual Bool GetVoltage(OutParam<Double> volt) = 0;
	};
}
#endif
