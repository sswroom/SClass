#ifndef _SM_IO_DEVICE_ED516
#define _SM_IO_DEVICE_ED516
#include "IO/MODBUSDevice.h"

namespace IO
{
	namespace Device
	{
		class ED516 : public IO::MODBUSDevice
		{
		public:
			ED516(NN<IO::MODBUSMaster> modbus, UInt8 addr);
			virtual ~ED516();

			Bool IsDIHighByCoil(UInt16 diNum);
			Bool IsDIHighByInput(UInt16 diNum);
			Bool IsDIHighByReg(UInt16 diNum);
			UInt16 GetDICountByReg(UInt16 diNum);
			Bool GetDICountByReg(UInt16 diNum, OutParam<Int32> val);
			UInt16 GetDICountByHolding(UInt16 diNum);
			Bool ClearDICount(UInt16 diNum);

			static void GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, AnyType userObj);
		};
	}
}
#endif
