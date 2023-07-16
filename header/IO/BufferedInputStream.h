#ifndef _SM_IO_BUFFEREDINPUTSTREAM
#define _SM_IO_BUFFEREDINPUTSTREAM
#include "Data/ByteBuffer.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class BufferedInputStream : public SeekableStream
	{
	private:
		IO::SeekableStream *stm;
		Data::ByteBuffer buff;
		UOSInt buffOfst;
		UInt64 stmPos;
		UOSInt stmBuffSize;

	public:
		BufferedInputStream(IO::SeekableStream *stm, UOSInt buffSize);
		virtual ~BufferedInputStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(Data::ByteArray buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(Data::ByteArray buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
	};
}
#endif
