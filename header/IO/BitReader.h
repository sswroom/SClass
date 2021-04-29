#ifndef _SM_IO_BITREADER
#define _SM_IO_BITREADER

namespace IO
{
	class BitReader
	{
	public:
		virtual ~BitReader() {};

		virtual Bool ReadBits(UInt32 *code, UOSInt bitCount) = 0;
		virtual Bool ByteAlign() = 0;
		virtual UOSInt ReadBytes(UInt8 *buff, UOSInt cnt) = 0;
	};
}
#endif
