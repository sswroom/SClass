#ifndef _SM_DATA_COMPRESS_LZWSTREAMDATADEC
#define _SM_DATA_COMPRESS_LZWSTREAMDATADEC
#include "IO/Stream.h"
#include "IO/StreamData.h"

namespace Data
{
	namespace Compress
	{
		class LZWStreamDataDec
		{

		public:
			static Bool Decode(NN<IO::StreamData> data, NN<IO::Stream> output, UIntOS earlyChange);
			static Bool Decode(NN<IO::Stream> stream, NN<IO::Stream> output, UIntOS earlyChange);
		};
	}
}

#endif
