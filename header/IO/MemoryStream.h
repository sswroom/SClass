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
		const UTF8Char *dbg;
	public:
		MemoryStream(const UTF8Char *dbg);
		MemoryStream(UOSInt capacity, const UTF8Char *dbg);
		MemoryStream(UInt8 *buff, UOSInt buffSize, const UTF8Char *dbg);
		virtual ~MemoryStream();

		UInt8 *GetBuff(UOSInt *buffSize);

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual UInt64 Seek(SeekType origin, Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();

		void Clear();
	};
}
#endif
