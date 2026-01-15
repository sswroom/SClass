#ifndef _SM_IO_I2CCHANNEL
#define _SM_IO_I2CCHANNEL

namespace IO
{
	class I2CChannel
	{
	public:
		virtual ~I2CChannel() {}

		virtual Bool IsError() = 0;

		virtual UIntOS I2CRead(UnsafeArray<UInt8> buff, UIntOS buffSize) = 0;
		virtual UIntOS I2CWrite(UnsafeArray<const UInt8> buff, UIntOS buffSize) = 0;
	};
}

#endif
