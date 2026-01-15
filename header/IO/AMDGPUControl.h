#ifndef _SM_IO_AMDGPUCONTROL
#define _SM_IO_AMDGPUCONTROL
#include "AnyType.h"
#include "IO/AMDGPUManager.h"

namespace IO
{
	class AMDGPUControl : public IO::GPUControl
	{
	private:
		NN<IO::AMDGPUManager> gpuMgr;
		AnyType adapter;
		IntOS odrivever;
		NN<Text::String> name;

	public:
		AMDGPUControl(NN<IO::AMDGPUManager> gpuMgr, AnyType adapter);
		virtual ~AMDGPUControl();

		virtual NN<Text::String> GetName();
		virtual Bool GetTemperature(OutParam<Double> temp);
		virtual Bool GetCoreClock(OutParam<Double> mhz);
		virtual Bool GetMemoryClock(OutParam<Double> mhz);
		virtual Bool GetVoltage(OutParam<Double> volt);
	};
}
#endif
