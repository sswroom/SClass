#ifndef _SM_IO_BITREADERMSB
#define _SM_IO_BITREADERMSB
#include "IO/BitReader.h"
#include "IO/Stream.h"

namespace IO
{
	class BitReaderMSB : public IO::BitReader
	{
	private:
		UInt8 *buff;
		OSInt buffSize;
		OSInt currBytePos;
		OSInt currBitPos;
		IO::Stream *stm;

	public:
		BitReaderMSB(IO::Stream *stm);
		BitReaderMSB(const UInt8 *buff, OSInt buffSize);
		virtual ~BitReaderMSB();

		virtual Bool ReadBits(Int32 *code, OSInt bitCount);
		virtual Bool ByteAlign();
		virtual OSInt ReadBytes(UInt8 *buff, OSInt cnt);
	};
};
#endif
