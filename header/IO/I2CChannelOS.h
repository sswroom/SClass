#ifndef _SM_IO_I2CCHANNELOS
#define _SM_IO_I2CCHANNELOS
#include "IO/I2CChannel.h"

namespace IO
{
	class I2CChannelOS : public IO::I2CChannel
	{
	private:
		void *hand;
	public:
		I2CChannelOS(Int32 busNo, UInt8 slaveAddr);
		virtual ~I2CChannelOS();

		virtual Bool IsError();
		
		virtual UOSInt I2CRead(UnsafeArray<UInt8> buff, UOSInt buffSize);
		virtual UOSInt I2CWrite(UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
