#ifndef _SM_IO_DEVICE_AMGU4241
#define _SM_IO_DEVICE_AMGU4241
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class AMGU4241 : public IO::MODBUSDevice
		{
		public:
			AMGU4241(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~AMGU4241();

			Bool ReadPeopleCount(OutParam<Int32> count);
			Bool ReadTemperature(OutParam<Double> temp);
			
			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, AnyType userObj);
		};
	}
}
#endif
