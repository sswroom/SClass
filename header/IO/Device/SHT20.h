#ifndef _SM_IO_DEVICE_SHT20
#define _SM_IO_DEVICE_SHT20
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class SHT20 : public IO::MODBUSDevice
		{
		public:
			SHT20(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~SHT20();

			Bool ReadTempRH(OutParam<Double> temp, OutParam<Double> rh);
			Bool ReadTemp(OutParam<Double> temp);
			Bool ReadRH(OutParam<Double> rh);

			Bool ReadBaudRate(OutParam<Int32> baudRate);
			Bool ReadParity(OutParam<Int32> parity);
			Bool ReadId(OutParam<Int32> id);
			Bool ReadIdValid(OutParam<Int32> idValid);

			Bool SetId(UInt8 id);
		};
	}
}
#endif
