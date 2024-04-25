#ifndef _SM_IO_DEVICE_TH10SB
#define _SM_IO_DEVICE_TH10SB
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class TH10SB : public IO::MODBUSDevice
		{
		public:
			TH10SB(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~TH10SB();

			Bool ReadTempRH(OutParam<Double> temp, OutParam<Double> rh);
			Bool ReadTemp(OutParam<Double> temp);
			Bool ReadRH(OutParam<Double> rh);

			Bool ReadId(OutParam<Int32> id);
			Bool ReadBaudRate(OutParam<Int32> baudRate);

			Bool SetId(UInt8 id);
			Bool SetBaudRate(Int32 baudRate);
		};
	}
}
#endif
