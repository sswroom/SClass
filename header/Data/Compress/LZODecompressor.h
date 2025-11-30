#ifndef _SM_DATA_COMPRESS_LZODECOMPRESSOR
#define _SM_DATA_COMPRESS_LZODECOMPRESSOR
#include "Data/Compress/Decompressor.h"

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
			virtual Bool Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData);
		};
	}
}

#endif
