#ifndef _SM_DATA_COMPRESS_DECOMPRESSOR
#define _SM_DATA_COMPRESS_DECOMPRESSOR
#include "IO/IStreamData.h"
#include "IO/Stream.h"
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

			virtual Bool Decompress(UInt8 *destBuff, UOSInt *destBuffSize, const UInt8 *srcBuff, UOSInt srcBuffSize) = 0;
			virtual Bool Decompress(IO::Stream *destStm, IO::IStreamData *srcData) = 0;

			static Data::Compress::Decompressor *CreateDecompressor(CompressMethod compMeth);
			static Text::CString GetCompMethName(CompressMethod compMethod);
		};
	}
}

#endif
