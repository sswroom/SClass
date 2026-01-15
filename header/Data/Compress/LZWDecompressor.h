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

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UIntOS> outDestBuffSize, Data::ByteArrayR srcBuff);
			virtual Bool Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData);
		};
	}
}

#endif
