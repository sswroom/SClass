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
		UOSInt buffSize;
		UOSInt currBytePos;
		UOSInt currBitPos;
		Optional<IO::Stream> stm;

	public:
		BitReaderLSB(NN<IO::Stream> stm);
		BitReaderLSB(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		virtual ~BitReaderLSB();

		virtual Bool ReadBits(OutParam<UInt32> code, UOSInt bitCount);
		virtual Bool ByteAlign();
		virtual UOSInt ReadBytes(UnsafeArray<UInt8> buff, UOSInt cnt);
	};
}
#endif
