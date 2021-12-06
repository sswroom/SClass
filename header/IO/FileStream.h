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
		enum class BufferType
		{
			RandomAccess,
			Normal,
			Sequential,
			NoBuffer,
			NoWriteBuffer
		};
		enum class FileMode
		{
			Create,
			Append,
			ReadOnly,
			ReadWriteExisting,
			Device,
			CreateWrite
		};
		enum class FileShare
		{
			DenyNone,
			DenyRead,
			DenyWrite,
			DenyAll
		};

	private:
		void InitStream(const WChar *fileName, FileMode mode, FileShare share, BufferType buffType);
	public:
		FileStream(Text::String *fileName, FileMode mode, FileShare share, BufferType buffType);
		FileStream(const UTF8Char *fileName, FileMode mode, FileShare share, BufferType buffType);
		virtual ~FileStream();

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
		virtual void GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
		virtual void SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);

		static IO::FileStream *CreateNamedPipe(const UTF8Char *pipeName, UInt32 buffSize);
		static IO::FileStream *OpenNamedPipe(const UTF8Char *server, const UTF8Char *pipeName);
		static UOSInt LoadFile(const UTF8Char *fileName, UInt8 *buff, UOSInt maxBuffSize);
	};
}
#endif
