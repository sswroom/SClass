#ifndef _SM_DATA_COMPRESS_LZODECOMPRESSOR
#define _SM_DATA_COMPRESS_LZODECOMPRESSOR
#include "Data/Compress/Decompressor.h"

extern "C"
{
	Bool LZODecompressor_Decompress(const UInt8 *in, UOSInt in_len, UInt8 *out, UOSInt *out_len);
	UOSInt LZODecompressor_CalcDecSize(const UInt8 *in, UOSInt in_len);
}

namespace Data
{
	namespace Compress
	{
		class LZODecompressor : public Decompressor
		{
		public:
			LZODecompressor();
			virtual ~LZODecompressor();

			virtual Bool Decompress(UInt8 *destBuff, UOSInt *destBuffSize, const UInt8 *srcBuff, UOSInt srcBuffSize);
			virtual Bool Decompress(IO::Stream *destStm, IO::StreamData *srcData);
		};
	}
}

#endif
