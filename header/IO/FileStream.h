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
		void InitStream(UnsafeArrayOpt<const WChar> fileName, FileMode mode, FileShare share, BufferType buffType);
	public:
		FileStream(NN<Text::String> fileName, FileMode mode, FileShare share, BufferType buffType);
		FileStream(Text::CStringNN fileName, FileMode mode, FileShare share, BufferType buffType);
		virtual ~FileStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
		void SetLength(UInt64 newLength);
		virtual StreamType GetStreamType() const;

		virtual Bool IsError() const;
		virtual Int32 GetErrCode();
		void GetFileTimes(Optional<Data::DateTime> creationTime, Optional<Data::DateTime> lastAccessTime, Optional<Data::DateTime> lastWriteTime);
		void GetFileTimes(OptOut<Data::Timestamp> creationTime, OptOut<Data::Timestamp> lastAccessTime, OptOut<Data::Timestamp> lastWriteTime);
		Data::Timestamp GetCreateTime();
		Data::Timestamp GetModifyTime();
		void SetFileTimes(Optional<Data::DateTime> creationTime, Optional<Data::DateTime> lastAccessTime, Optional<Data::DateTime> lastWriteTime);
		void SetFileTimes(const Data::Timestamp &creationTime, const Data::Timestamp &lastAccessTime, const Data::Timestamp &lastWriteTime);

		static Optional<IO::FileStream> CreateNamedPipe(UnsafeArray<const UTF8Char> pipeName, UInt32 buffSize);
		static Optional<IO::FileStream> OpenNamedPipe(UnsafeArrayOpt<const UTF8Char> server, UnsafeArray<const UTF8Char> pipeName);
		static UIntOS LoadFile(Text::CStringNN fileName, UnsafeArray<UInt8> buff, UIntOS maxBuffSize);
	};
}
#endif
