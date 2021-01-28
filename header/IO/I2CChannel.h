#ifndef _SM_IO_I2CCHANNEL
#define _SM_IO_I2CCHANNEL

namespace IO
{
	class I2CChannel
	{
	public:
		virtual ~I2CChannel() {}

		virtual Bool IsError() = 0;

		virtual OSInt I2CRead(UInt8 *buff, OSInt buffSize) = 0;
		virtual OSInt I2CWrite(const UInt8 *buff, OSInt buffSize) = 0;
	};
};

#endif
