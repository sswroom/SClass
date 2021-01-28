#ifndef _SM_IO_IGPUCONTROL
#define _SM_IO_IGPUCONTROL

namespace IO
{
	class IGPUControl
	{
	public:
		virtual ~IGPUControl() {}

		virtual const UTF8Char *GetName() = 0;
		virtual Bool GetTemperature(Double *temp) = 0;
		virtual Bool GetCoreClock(Double *mhz) = 0;
		virtual Bool GetMemoryClock(Double *mhz) = 0;
		virtual Bool GetVoltage(Double *volt) = 0;
	};
};
#endif
