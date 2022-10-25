#ifndef _SM_IO_FILESTREAM
#define _SM_IO_FILESTREAM
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "IO/FileMode.h"
#include "IO/SeekableStream.h"
#include "Text/CString.h"

namespace IO
{
	class FileStream : public SeekableStream
	{
	protected:
		void *handle;
		UInt64 currPos;

		FileStream();
	public:
		enum class BufferType
		{
			RandomAccess,
			Normal,
			Sequential,
			NoBuffer,
			NoWriteBuffer
		};

	private:
		void InitStream(const WChar *fileName, FileMode mode, FileShare share, BufferType buffType);
	public:
		FileStream(Text::String *fileName, FileMode mode, FileShare share, BufferType buffType);
		FileStream(Text::CString fileName, FileMode mode, FileShare share, BufferType buffType);
		virtual ~FileStream();

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
		void SetLength(UInt64 newLength);

		virtual Bool IsError();
		virtual Int32 GetErrCode();
		void GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
		void GetFileTimes(Data::Timestamp *creationTime, Data::Timestamp *lastAccessTime, Data::Timestamp *lastWriteTime);
		Data::Timestamp GetCreateTime();
		Data::Timestamp GetModifyTime();
		void SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
		void SetFileTimes(Data::Timestamp creationTime, Data::Timestamp lastAccessTime, Data::Timestamp lastWriteTime);

		static IO::FileStream *CreateNamedPipe(const UTF8Char *pipeName, UInt32 buffSize);
		static IO::FileStream *OpenNamedPipe(const UTF8Char *server, const UTF8Char *pipeName);
		static UOSInt LoadFile(Text::CString fileName, UInt8 *buff, UOSInt maxBuffSize);
	};
}
#endif
