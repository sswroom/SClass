#ifndef _SM_IO_ONEWIREGPIO
#define _SM_IO_ONEWIREGPIO
#include "IO/IOPin.h"
#include "Manage/HiResClock.h"

namespace IO
{
	class OneWireGPIO
	{
	private:
		Manage::HiResClock *clk;
		IO::IOPin *pin;
	public:
		OneWireGPIO(IO::IOPin *pin);
		~OneWireGPIO();

		Bool Init();

		void SendBits(const UInt8 *buff, OSInt nBits);
		OSInt ReadBits(UInt8 *buff, OSInt nBits);
	};
}
#endif
