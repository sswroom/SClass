#ifndef _SM_IO_BITWRITERMSB
#define _SM_IO_BITWRITERMSB
#include "IO/BitWriter.h"
#include "IO/Stream.h"

namespace IO
{
	class BitWriterMSB : public IO::BitWriter
	{
	private:
		UInt8 *buff;
		OSInt currBytePos;
		OSInt currBitPos;
		IO::Stream *stm;

	public:
		BitWriterMSB(IO::Stream *stm);
		virtual ~BitWriterMSB();

		virtual Bool WriteBits(UInt32 code, OSInt bitCount);
		virtual Bool ByteAlign();
	};
};
#endif
