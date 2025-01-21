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
			enum class CompressionLevel
			{
				NoCompression = 0,
				BestSpeed = 1,
				Level2 = 2,
				Level3 = 3,
				Level4 = 4,
				Level5 = 5,
				DefaultCompression = 6,
				Level7 = 7,
				Level8 = 8,
				BestCompression = 9,
				UberCompression = 10
			};
		private:
			Bool hasHeader;
		public:
			Inflate(Bool hasHeader);
			virtual ~Inflate();

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff);
			virtual Bool Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData);

			static UOSInt TestCompress(UnsafeArray<const UInt8> srcBuff, UOSInt srcBuffSize, Bool hasHeader);
			static UOSInt Compress(UnsafeArray<const UInt8> srcBuff, UOSInt srcBuffSize, UnsafeArray<UInt8> destBuff, Bool hasHeader, CompressionLevel level);
		};
	}
}
#endif
