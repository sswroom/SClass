#ifndef _SM_IO_BITWRITERLSB
#define _SM_IO_BITWRITERLSB
#include "IO/BitWriter.h"
#include "IO/Stream.h"

namespace IO
{
	class BitWriterLSB : public IO::BitWriter
	{
	private:
		UnsafeArray<UInt8> buff;
		UIntOS currBytePos;
		UIntOS currBitPos;
		NN<IO::Stream> stm;

	public:
		BitWriterLSB(NN<IO::Stream> stm);
		virtual ~BitWriterLSB();

		virtual Bool WriteBits(UInt32 code, UIntOS bitCount);
		virtual Bool ByteAlign();
	};
}
#endif
