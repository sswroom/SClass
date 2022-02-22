#ifndef _SM_IO_STMDATA_FILEDATA
#define _SM_IO_STMDATA_FILEDATA
#include "IO/IStreamData.h"
#include "IO/FileStream.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

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
				Text::CString fileName;
				Text::String *fullName;
				Text::String *filePath;
				Bool deleteOnClose;
				UOSInt seekCnt;

				Sync::Mutex *mut;
				UInt32 objectCnt;
			} FILEDATAHANDLE;

			typedef struct
			{
				Text::CString fileName;
				Text::String *fullName;
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
			FileData(Text::String *fileName, Bool deleteOnClose);
			FileData(Text::CString fileName, Bool deleteOnClose);
			virtual ~FileData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual Text::String *GetFullName();
			virtual Text::CString GetShortName();
			virtual void SetFullName(Text::CString fullName);
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Text::String *GetFullFileName();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

			IO::FileStream *GetFileStream();
			Bool IsError();
		private:
			void Close();
		};
	}
}
#endif
