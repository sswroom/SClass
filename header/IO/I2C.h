#ifndef _SM_IO_I2C
#define _SM_IO_I2C
#include "IO/I2CChannel.h"

namespace IO
{
	class I2C
	{
	protected:
		IO::I2CChannel *channel;
		Int32 delayMS;
	public:
		I2C(IO::I2CChannel *channel, Int32 delayMS);
		virtual ~I2C();

		void Wait();
		virtual Bool ReadBuff(UInt8 regAddr, UInt8 len, UInt8 *data);
		virtual Bool WriteBuff(UInt8 regAddr, UInt8 len, UInt8 *data);
	};
};

#endif
