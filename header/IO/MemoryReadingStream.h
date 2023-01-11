#ifndef _SM_IO_MEMORYREADINGSTREAM
#define _SM_IO_MEMORYREADINGSTREAM
#include "IO/SeekableStream.h"

namespace IO
{
	class MemoryReadingStream : public SeekableStream
	{
	private:
		const UInt8 *buff;
		UOSInt buffSize;
		UOSInt currPtr;
	public:
		MemoryReadingStream(const UInt8 *buff, UOSInt buffSize);
		virtual ~MemoryReadingStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

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
