#ifndef _SM_DATA_COMPRESS_INFLATE
#define _SM_DATA_COMPRESS_INFLATE
#include "Data/Compress/Decompressor.h"
#include "Data/Compress/Deflater.h"

namespace Data
{
	namespace Compress
	{
		class Inflate : public Decompressor
		{
		private:
			Bool hasHeader;
		public:
			Inflate(Bool hasHeader);
			virtual ~Inflate();

			virtual Bool Decompress(Data::ByteArray destBuff, OutParam<UIntOS> outDestBuffSize, Data::ByteArrayR srcBuff);
			virtual Bool Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData);

			static UIntOS TestCompress(UnsafeArray<const UInt8> srcBuff, UIntOS srcBuffSize, Bool hasHeader);
			static UIntOS Compress(UnsafeArray<const UInt8> srcBuff, UIntOS srcBuffSize, UnsafeArray<UInt8> destBuff, Bool hasHeader, Data::Compress::Deflater::CompLevel level);
		};
	}
}
#endif
