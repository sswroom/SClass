#ifndef _SM_IO_DEVICE_ED527
#define _SM_IO_DEVICE_ED527
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class ED527 : public IO::MODBUSDevice
		{
		public:
			ED527(NotNullPtr<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~ED527();

			Bool IsRelayHigh(UInt16 index);
			Bool SetRelayState(UInt16 index, Bool isHigh);
			Bool GetOutputOverloadFlag(UInt16 diNum);

			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, AnyType userObj);
		};
	}
}
#endif
