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
			TH10SB(IO::MODBUSMaster *modbus, UInt8 addr);
			virtual ~TH10SB();

			Bool ReadTempRH(Double *temp, Double *rh);
			Bool ReadTemp(Double *temp);
			Bool ReadRH(Double *rh);

			Bool ReadId(Int32 *id);
			Bool ReadBaudRate(Int32 *baudRate);

			Bool SetId(UInt8 id);
			Bool SetBaudRate(Int32 baudRate);
		};
	}
}
#endif
