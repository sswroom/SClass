#ifndef _SM_IO_BITWRITERMSB
#define _SM_IO_BITWRITERMSB
#include "IO/BitWriter.h"
#include "IO/Stream.h"

namespace IO
{
	class BitWriterMSB : public IO::BitWriter
	{
	private:
		UnsafeArray<UInt8> buff;
		UOSInt currBytePos;
		UOSInt currBitPos;
		NN<IO::Stream> stm;

	public:
		BitWriterMSB(NN<IO::Stream> stm);
		virtual ~BitWriterMSB();

		virtual Bool WriteBits(UInt32 code, UOSInt bitCount);
		virtual Bool ByteAlign();
	};
}
#endif
