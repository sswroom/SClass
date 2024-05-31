#ifndef _SM_IO_WRITECACHESTREAM
#define _SM_IO_WRITECACHESTREAM
#include "IO/Stream.h"

namespace IO
{
	class WriteCacheStream : public IO::Stream
	{
	private:
		NN<IO::Stream> outStm;
		UInt8 *cacheBuff;
		UOSInt cacheBuffSize;
		UOSInt cacheSize;
	public:
		WriteCacheStream(NN<IO::Stream> outStm);
		virtual ~WriteCacheStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
