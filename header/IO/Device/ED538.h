#ifndef _SM_IO_DEVICE_ED538
#define _SM_IO_DEVICE_ED538
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class ED538 : public IO::MODBUSDevice
		{
		public:
			ED538(IO::MODBUSMaster *modbus, UInt8 addr);
			virtual ~ED538();

			Bool IsDIHighByCoil(UInt16 diNum);
			Bool IsDIHighByInput(UInt16 diNum);
			Bool IsDIHighByReg(UInt16 diNum);
			UInt16 GetDICountByReg(UInt16 diNum);
			Bool GetDICountByReg(UInt16 diNum, Int32 *val);
			UInt16 GetDICountByHolding(UInt16 diNum);
			Bool ClearDICount(UInt16 diNum);
			Bool IsRelayHigh(UInt16 index);
			Bool SetRelayState(UInt16 index, Bool isHigh);
			Bool GetOutputOverloadFlag(UInt16 diNum);

			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj);
		};
	}
}
#endif
