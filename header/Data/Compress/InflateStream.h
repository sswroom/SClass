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
			InflateStream(IO::Stream *outStm, UOSInt headerSize);
			InflateStream(IO::Stream *outStm);
			virtual ~InflateStream();

			virtual Bool IsDown();
			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}

#endif
