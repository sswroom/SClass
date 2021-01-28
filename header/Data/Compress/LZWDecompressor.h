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

			virtual Bool Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize);
			virtual Bool Decompress(IO::Stream *destStm, IO::IStreamData *srcData);
		};
	}
}

#endif
