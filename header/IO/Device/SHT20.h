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
			SHT20(IO::MODBUSMaster *modbus, UInt8 addr);
			virtual ~SHT20();

			Bool ReadTempRH(Double *temp, Double *rh);
			Bool ReadTemp(Double *temp);
			Bool ReadRH(Double *rh);

			Bool ReadBaudRate(Int32 *baudRate);
			Bool ReadParity(Int32 *parity);
			Bool ReadId(Int32 *id);
			Bool ReadIdValid(Int32 *idValid);

			Bool SetId(UInt8 id);
		};
	}
}
#endif
