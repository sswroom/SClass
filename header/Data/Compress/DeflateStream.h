#ifndef _SM_DATA_COMPRESS_DEFLATESTREAM
#define _SM_DATA_COMPRESS_DEFLATESTREAM
#include "Crypto/Hash/IHash.h"
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		class DeflateStream : public IO::Stream
		{
		private:
			struct ClassData;
			ClassData *clsData;
		public:
			DeflateStream(IO::Stream *srcStm, UInt64 srcLeng, Crypto::Hash::IHash *hash, Bool hasHeader);
			virtual ~DeflateStream();

			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}

#endif
