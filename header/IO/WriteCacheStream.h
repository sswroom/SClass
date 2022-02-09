#ifndef _SM_IO_WRITECACHESTREAM
#define _SM_IO_WRITECACHESTREAM
#include "IO/Stream.h"

namespace IO
{
	class WriteCacheStream : public IO::Stream
	{
	private:
		IO::Stream *outStm;
		UInt8 *cacheBuff;
		UOSInt cacheBuffSize;
		UOSInt cacheSize;
	public:
		WriteCacheStream(IO::Stream *outStm);
		virtual ~WriteCacheStream();

		virtual Bool IsDown();
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
