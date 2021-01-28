#ifndef _SM_IO_BITWRITER
#define _SM_IO_BITWRITER

namespace IO
{
	class BitWriter
	{
	public:
		virtual ~BitWriter() {};

		virtual Bool WriteBits(UInt32 code, OSInt bitCount) = 0;
		virtual Bool ByteAlign() = 0;
	};
};
#endif
