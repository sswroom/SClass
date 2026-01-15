#ifndef _SM_IO_BITREADERLSB
#define _SM_IO_BITREADERLSB
#include "IO/BitReader.h"
#include "IO/Stream.h"

namespace IO
{
	class BitReaderLSB : public IO::BitReader
	{
	private:
		UnsafeArray<UInt8> buff;
		UIntOS buffSize;
		UIntOS currBytePos;
		UIntOS currBitPos;
		Optional<IO::Stream> stm;

	public:
		BitReaderLSB(NN<IO::Stream> stm);
		BitReaderLSB(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		virtual ~BitReaderLSB();

		virtual Bool ReadBits(OutParam<UInt32> code, UIntOS bitCount);
		virtual Bool ByteAlign();
		virtual UIntOS ReadBytes(UnsafeArray<UInt8> buff, UIntOS cnt);
	};
}
#endif
