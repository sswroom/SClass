#ifndef _SM_IO_BITREADER
#define _SM_IO_BITREADER

namespace IO
{
	class BitReader
	{
	public:
		virtual ~BitReader() {};

		virtual Bool ReadBits(OutParam<UInt32> code, UIntOS bitCount) = 0;
		virtual Bool ByteAlign() = 0;
		virtual UIntOS ReadBytes(UnsafeArray<UInt8> buff, UIntOS cnt) = 0;
	};
}
#endif
