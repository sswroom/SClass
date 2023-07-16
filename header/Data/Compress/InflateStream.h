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
			IO::Stream *outStm;
			void *cmpInfo;
			UInt8 *writeBuff;
			UOSInt headerSize;
		public:
			InflateStream(IO::Stream *outStm, UOSInt headerSize, Bool zlibHeader);
			InflateStream(IO::Stream *outStm, Bool zlibHeader);
			virtual ~InflateStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(Data::ByteArray buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}

#endif
