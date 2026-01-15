#ifndef _SM_DATA_COMPRESS_DEFLATESTREAM
#define _SM_DATA_COMPRESS_DEFLATESTREAM
#include "Crypto/Hash/HashAlgorithm.h"
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		class DeflateStream : public IO::Stream
		{
		public:
			enum class CompLevel
			{
				Default,
				MaxCompression,
				MaxSpeed
			};
		private:
			struct ClassData;
			NN<ClassData> clsData;
		public:
			DeflateStream(NN<IO::Stream> srcStm, UInt64 srcLeng, Optional<Crypto::Hash::HashAlgorithm> hash, CompLevel level, Bool hasHeader);
			virtual ~DeflateStream();

			virtual Bool IsDown() const;
			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}

#endif
