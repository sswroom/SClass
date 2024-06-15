#ifndef _SM_IO_BITREADERMSB
#define _SM_IO_BITREADERMSB
#include "IO/BitReader.h"
#include "IO/Stream.h"

namespace IO
{
	class BitReaderMSB : public IO::BitReader
	{
	private:
		UnsafeArray<UInt8> buff;
		UOSInt buffSize;
		UOSInt currBytePos;
		UOSInt currBitPos;
		Optional<IO::Stream> stm;

	public:
		BitReaderMSB(NN<IO::Stream> stm);
		BitReaderMSB(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		virtual ~BitReaderMSB();

		virtual Bool ReadBits(OutParam<UInt32> code, UOSInt bitCount);
		virtual Bool ByteAlign();
		virtual UOSInt ReadBytes(UInt8 *buff, UOSInt cnt);
	};
}
#endif
