#ifndef _SM_IO_MEMORYREADINGSTREAM
#define _SM_IO_MEMORYREADINGSTREAM
#include "IO/SeekableStream.h"

namespace IO
{
	class MemoryReadingStream : public SeekableStream
	{
	private:
		Data::ByteArrayR buff;
		UOSInt currPtr;
	public:
		MemoryReadingStream(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		MemoryReadingStream(Data::ByteArrayR buff);
		virtual ~MemoryReadingStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
	};
}
#endif
