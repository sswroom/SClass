#ifndef _SM_DATA_COMPRESS_DECOMPRESSOR
#define _SM_DATA_COMPRESS_DECOMPRESSOR
#include "IO/Stream.h"
#include "IO/StreamData.h"
#include "Text/CString.h"

namespace Data
{
	namespace Compress
	{
		class Decompressor
		{
		public:
			typedef enum
			{
				CM_UNKNOWN,
				CM_UNCOMPRESSED,
				CM_DEFLATE,
				CM_MLH,
				CM_LZO
			} CompressMethod;

		public:
			virtual ~Decompressor();

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UIntOS> outDestBuffSize, Data::ByteArrayR srcBuff) = 0;
			virtual Bool Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData) = 0;

			static Optional<Data::Compress::Decompressor> CreateDecompressor(CompressMethod compMeth);
			static Text::CStringNN GetCompMethName(CompressMethod compMethod);
		};
	}
}

#endif
