#ifndef _SM_IO_FILEVIEWSTREAM
#define _SM_IO_FILEVIEWSTREAM
#include "IO/SeekableStream.h"
#include "IO/ViewFileBuffer.h"

namespace IO
{
	class FileViewStream : public SeekableStream
	{
	private:
		IO::ViewFileBuffer *vfb;
		UInt64 length;
		UInt64 currPos;
		UInt8 *fptr;

	public:
		FileViewStream(Text::CString fileName);
		virtual ~FileViewStream();

		virtual Bool IsDown();
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();

		virtual Bool IsError();
	};
}
#endif
