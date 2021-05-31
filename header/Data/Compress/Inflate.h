#ifndef _SM_DATA_COMPRESS_INFLATE
#define _SM_DATA_COMPRESS_INFLATE
#include "Data/Compress/Decompressor.h"

namespace Data
{
	namespace Compress
	{
		class Inflate : public Decompressor
		{
		public:
			Inflate();
			virtual ~Inflate();

			virtual Bool Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize);
			virtual Bool Decompress(IO::Stream *destStm, IO::IStreamData *srcData);

			static UOSInt TestCompress(const UInt8 *srcBuff, UOSInt srcBuffSize, Bool hasHeader);
			static UOSInt Compress(const UInt8 *srcBuff, UOSInt srcBuffSize, UInt8 *destBuff, Bool hasHeader);
		};
	}
}
#endif
