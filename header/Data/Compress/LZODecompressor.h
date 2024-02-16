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

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff);
			virtual Bool Decompress(NotNullPtr<IO::Stream> destStm, NotNullPtr<IO::StreamData> srcData);
		};
	}
}

#endif
