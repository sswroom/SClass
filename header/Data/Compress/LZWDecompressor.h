#ifndef _SM_DATA_COMPRESS_LZWDECOMPRESSOR
#define _SM_DATA_COMPRESS_LZWDECOMPRESSOR
#include "Data/Compress/Decompressor.h"

namespace Data
{
	namespace Compress
	{
		class LZWDecompressor : public Decompressor
		{
		public:
			LZWDecompressor();
			virtual ~LZWDecompressor();

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff);
			virtual Bool Decompress(NotNullPtr<IO::Stream> destStm, NotNullPtr<IO::StreamData> srcData);
		};
	}
}

#endif
