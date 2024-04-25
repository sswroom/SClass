#ifndef _SM_IO_DEVICE_BIVOCOMTD210
#define _SM_IO_DEVICE_BIVOCOMTD210
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class BivocomTD210 : public IO::MODBUSDevice
		{
		public:
			BivocomTD210(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~BivocomTD210();

			Bool ReadIOs(Int32 *io); //Bit0 = IO1, Bit1 = IO@, Bit2 = IO3
			Bool SetOutput1(Bool isHigh);
			Bool SetOutput2(Bool isHigh);
			Bool SetOutput3(Bool isHigh);
			Bool ReadADC1(OutParam<Int32> adc);
			Bool ReadADC2(OutParam<Int32> adc);
		};
	}
}
#endif
