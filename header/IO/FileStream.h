#ifndef _SM_IO_FILESTREAM
#define _SM_IO_FILESTREAM
#include "Data/DateTime.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class FileStream : public SeekableStream
	{
	protected:
		void *handle;
		UInt64 currPos;

		FileStream();
	public:
		enum BufferType
		{
			BT_RANDOM_ACCESS,
			BT_NORMAL,
			BT_SEQUENTIAL,
			BT_NO_BUFFER,
			BT_NO_WRITE_BUFFER
		} ;
		enum FileMode
		{
			FILE_MODE_CREATE,
			FILE_MODE_APPEND,
			FILE_MODE_READONLY,
			FILE_MODE_READWRITEEXISTING,
			FILE_MODE_DEVICE,
			FILE_MODE_CREATEWRITE
		};
		enum FileShare
		{
			FILE_SHARE_DENY_NONE,
			FILE_SHARE_DENY_READ,
			FILE_SHARE_DENY_WRITE,
			FILE_SHARE_DENY_ALL
		};

	private:
		void InitStream(const WChar *fileName, FileMode mode, FileShare share, BufferType buffType);
	public:
		FileStream(const UTF8Char *fileName, FileMode mode, FileShare share, BufferType buffType);
		virtual ~FileStream();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual UInt64 Seek(SeekType origin, Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
		void SetLength(UInt64 newLength);

		virtual Bool IsError();
		virtual Int32 GetErrCode();
		virtual void GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
		virtual void SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);

		static IO::FileStream *CreateNamedPipe(const UTF8Char *pipeName, UInt32 buffSize);
		static IO::FileStream *OpenNamedPipe(const UTF8Char *server, const UTF8Char *pipeName);
	};
}
#endif
