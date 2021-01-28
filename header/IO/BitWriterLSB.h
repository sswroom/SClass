#ifndef _SM_IO_BITWRITERLSB
#define _SM_IO_BITWRITERLSB
#include "IO/BitWriter.h"
#include "IO/Stream.h"

namespace IO
{
	class BitWriterLSB : public IO::BitWriter
	{
	private:
		UInt8 *buff;
		OSInt currBytePos;
		OSInt currBitPos;
		IO::Stream *stm;

	public:
		BitWriterLSB(IO::Stream *stm);
		virtual ~BitWriterLSB();

		virtual Bool WriteBits(UInt32 code, OSInt bitCount);
		virtual Bool ByteAlign();
	};
};
#endif
