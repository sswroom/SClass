#ifndef _SM_DATA_COMPRESS_INFLATESTREAM
#define _SM_DATA_COMPRESS_INFLATESTREAM
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		class InflateStream : public IO::Stream
		{
		private:
			NotNullPtr<IO::Stream> outStm;
			void *cmpInfo;
			UInt8 *writeBuff;
			UOSInt headerSize;
		public:
			InflateStream(NotNullPtr<IO::Stream> outStm, UOSInt headerSize, Bool zlibHeader);
			InflateStream(NotNullPtr<IO::Stream> outStm, Bool zlibHeader);
			virtual ~InflateStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}

#endif
