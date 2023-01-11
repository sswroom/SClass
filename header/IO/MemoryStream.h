#ifndef _SM_IO_MEMORYSTREAM
#define _SM_IO_MEMORYSTREAM
#include "IO/SeekableStream.h"

namespace IO
{
	class MemoryStream : public SeekableStream
	{
	private:
		UOSInt capacity;
		UOSInt currPtr;
		UOSInt currSize;
		UInt8 *memPtr;
	public:
		MemoryStream();
		MemoryStream(UOSInt capacity);
		virtual ~MemoryStream();

		UInt8 *GetBuff();
		UInt8 *GetBuff(UOSInt *buffSize);

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

		void Clear();
	};
}
#endif
