#ifndef _SM_IO_AMDGPUCONTROL
#define _SM_IO_AMGGPUCONTROL
#include "IO/AMDGPUManager.h"

namespace IO
{
	class AMDGPUControl : public IO::IGPUControl
	{
	private:
		IO::AMDGPUManager *gpuMgr;
		void *adapter;
		OSInt odrivever;
		const UTF8Char *name;

	public:
		AMDGPUControl(IO::AMDGPUManager *gpuMgr, void *adapter);
		virtual ~AMDGPUControl();

		virtual const UTF8Char *GetName();
		virtual Bool GetTemperature(Double *temp);
		virtual Bool GetCoreClock(Double *mhz);
		virtual Bool GetMemoryClock(Double *mhz);
		virtual Bool GetVoltage(Double *volt);
	};
};
#endif
