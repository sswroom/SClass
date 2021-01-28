#ifndef _SM_IO_BITREADER
#define _SM_IO_BITREADER

namespace IO
{
	class BitReader
	{
	public:
		virtual ~BitReader() {};

		virtual Bool ReadBits(Int32 *code, OSInt bitCount) = 0;
		virtual Bool ByteAlign() = 0;
		virtual OSInt ReadBytes(UInt8 *buff, OSInt cnt) = 0;
	};
};
#endif
