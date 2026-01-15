#ifndef _SM_IO_ONEWIREGPIO
#define _SM_IO_ONEWIREGPIO
#include "IO/IOPin.h"
#include "Manage/HiResClock.h"

namespace IO
{
	class OneWireGPIO
	{
	private:
		Manage::HiResClock clk;
		NN<IO::IOPin> pin;
	public:
		OneWireGPIO(NN<IO::IOPin> pin);
		~OneWireGPIO();

		Bool Init();

		void SendBits(const UInt8 *buff, IntOS nBits);
		IntOS ReadBits(UInt8 *buff, IntOS nBits);
	};
}
#endif
