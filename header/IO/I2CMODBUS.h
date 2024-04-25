#ifndef _SM_IO_I2CMODBUS
#define _SM_IO_I2CMODBUS
#include "Crypto/Hash/CRC16R.h"
#include "IO/I2C.h"

namespace IO
{
	class I2CMODBUS : public I2C
	{
	private:
		Crypto::Hash::CRC16R crc;
	public:
		I2CMODBUS(NN<IO::I2CChannel> channel, UOSInt delayMS);
		virtual ~I2CMODBUS();

		virtual Bool ReadBuff(UInt8 regAddr, UInt8 len, UInt8 *data);
		virtual Bool WriteBuff(UInt8 regAddr, UInt8 len, UInt8 *data);
	};
}
#endif
