#ifndef _SM_IO_NULLSTREAM
#define _SM_IO_NULLSTREAM
#include "IO/SeekableStream.h"

namespace IO
{
	class NullStream : public SeekableStream
	{
	public:
		NullStream();
		virtual ~NullStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(Data::ByteArray buff);
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
