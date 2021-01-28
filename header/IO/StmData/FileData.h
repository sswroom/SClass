#ifndef _SM_IO_STMDATA_FILEDATA
#define _SM_IO_STMDATA_FILEDATA
#include "IO/IStreamData.h"
#include "IO/FileStream.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace StmData
	{
		class FileData : public IO::IStreamData
		{
		private:
			typedef struct
			{
				IO::FileStream *file;
				UInt64 fileLength;
				UInt64 currentOffset;
				UTF8Char *fileName;
				UTF8Char *fullName;
				const UTF8Char *filePath;
				Bool deleteOnClose;
				OSInt seekCnt;

				Sync::Mutex *mut;
				UInt32 objectCnt;
			} FILEDATAHANDLE;

			typedef struct
			{
				const UTF8Char *fileName;
				const UTF8Char *fullName;
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
			FileData(const UTF8Char *fileName, Bool deleteOnClose);
			virtual ~FileData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual const UTF8Char *GetFullName();
			virtual const UTF8Char *GetShortName();
			virtual void SetFullName(const UTF8Char *fullName);
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual const UTF8Char *GetFullFileName();
			virtual Bool IsLoading();
			virtual OSInt GetSeekCount();

			IO::FileStream *GetFileStream();
		private:
			void Close();
		};
	}
}
#endif
