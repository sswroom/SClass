#ifndef _SM_IO_WATCHDOG
#define _SM_IO_WATCHDOG

namespace IO
{
	class Watchdog
	{
	public:
		virtual ~Watchdog() {};

		virtual Bool IsError() = 0;

		virtual Bool Keepalive() = 0;
		virtual Bool Enable() = 0;
		virtual Bool Disable() = 0;
		virtual Bool SetTimeoutSec(Int32 timeoutSec) = 0;
		virtual Bool GetTimeoutSec(OutParam<Int32> timeoutSec) = 0;
		virtual Bool GetTemperature(OutParam<Double> temp) = 0;

		static Optional<Watchdog> Create();
		static Optional<Watchdog> Create(Int32 devNum);
	};
}
#endif
