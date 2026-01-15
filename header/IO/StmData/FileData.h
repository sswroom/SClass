#ifndef _SM_IO_STMDATA_FILEDATA
#define _SM_IO_STMDATA_FILEDATA
#include "IO/StreamData.h"
#include "IO/FileStream.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace IO
{
	namespace StmData
	{
		class FileData : public IO::StreamData
		{
		private:
			struct FILEDATAHANDLE
			{
				IO::FileStream *file;
				UInt64 fileLength;
				UInt64 currentOffset;
				Text::CString fileName;
				NN<Text::String> fullName;
				NN<Text::String> filePath;
				Bool deleteOnClose;
				UIntOS seekCnt;

				Sync::Mutex mut;
				UInt32 objectCnt;
			};

			typedef struct
			{
				Text::CString fileName;
				NN<Text::String> fullName;
				UInt32 objectCnt;
			} FILEDATANAME;

		private:
			FILEDATAHANDLE *fdh;
			FILEDATANAME *fdn;
			UInt64 dataOffset;
			UInt64 dataLength;

			void ReopenFile();
		public:
			FileData(const FileData *fd, UInt64 offset, UInt64 length);
			FileData(NN<Text::String> fileName, Bool deleteOnClose);
			FileData(Text::CStringNN fileName, Bool deleteOnClose);
			virtual ~FileData();

			virtual UIntOS GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName() const;
			virtual Text::CString GetShortName() const;
			virtual void SetFullName(Text::CStringNN fullName);
			virtual UInt64 GetDataSize() const;
			virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile() const;
			virtual NN<Text::String> GetFullFileName() const;
			virtual Bool IsLoading() const;
			virtual UIntOS GetSeekCount() const;

			IO::FileStream *GetFileStream();
			Bool IsError();
		private:
			void Close();
		};
	}
}
#endif
