#ifndef _SM_IO_BUFFEREDOUTPUTSTREAM
#define _SM_IO_WRIT_SM_IO_BUFFEREDOUTPUTSTREAMECACHESTREAM
#include "IO/Stream.h"

namespace IO
{
	class BufferedOutputStream : public IO::Stream
	{
	private:
		IO::Stream *outStm;
		UInt8 *cacheBuff;
		UOSInt cacheBuffSize;
		UOSInt cacheSize;
	public:
		BufferedOutputStream(IO::Stream *outStm, UOSInt buffSize);
		virtual ~BufferedOutputStream();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
