#ifndef _SM_IO_BUFFEREDOUTPUTSTREAM
#define _SM_IO_BUFFEREDOUTPUTSTREAM
#include "IO/Stream.h"

namespace IO
{
	class BufferedOutputStream : public IO::Stream
	{
	private:
		NN<IO::Stream> outStm;
		UnsafeArray<UInt8> cacheBuff;
		UIntOS cacheBuffSize;
		UIntOS cacheSize;
		UInt64 totalWrite;
	public:
		BufferedOutputStream(NN<IO::Stream> outStm, UIntOS buffSize);
		virtual ~BufferedOutputStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;

		UInt64 GetPosition() const;
	};
}
#endif
