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
		UOSInt buffSize;
		UOSInt currBytePos;
		UOSInt currBitPos;
		IO::Stream *stm;

	public:
		BitReaderLSB(IO::Stream *stm);
		BitReaderLSB(const UInt8 *buff, UOSInt buffSize);
		virtual ~BitReaderLSB();

		virtual Bool ReadBits(UInt32 *code, UOSInt bitCount);
		virtual Bool ByteAlign();
		virtual UOSInt ReadBytes(UInt8 *buff, UOSInt cnt);
	};
}
#endif
