#ifndef _SM_IO_BITREADERLSB
#define _SM_IO_BITREADERLSB
#include "IO/BitReader.h"
#include "IO/Stream.h"

namespace IO
{
	class BitReaderLSB : public IO::BitReader
	{
	private:
		UInt8 *buff;
		OSInt buffSize;
		OSInt currBytePos;
		OSInt currBitPos;
		IO::Stream *stm;

	public:
		BitReaderLSB(IO::Stream *stm);
		BitReaderLSB(const UInt8 *buff, OSInt buffSize);
		virtual ~BitReaderLSB();

		virtual Bool ReadBits(Int32 *code, OSInt bitCount);
		virtual Bool ByteAlign();
		virtual OSInt ReadBytes(UInt8 *buff, OSInt cnt);
	};
};
#endif
