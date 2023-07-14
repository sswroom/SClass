#ifndef _SM_IO_BUFFEREDOUTPUTSTREAM
#define _SM_IO_BUFFEREDOUTPUTSTREAM
#include "IO/Stream.h"

namespace IO
{
	class BufferedOutputStream : public IO::Stream
	{
	private:
		NotNullPtr<IO::Stream> outStm;
		UInt8 *cacheBuff;
		UOSInt cacheBuffSize;
		UOSInt cacheSize;
	public:
		BufferedOutputStream(NotNullPtr<IO::Stream> outStm, UOSInt buffSize);
		virtual ~BufferedOutputStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
