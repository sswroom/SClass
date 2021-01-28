#ifndef _SM_IO_STMDATA_FILEVIEWDATA
#define _SM_IO_STMDATA_FILEVIEWDATA
#include "IO/IStreamData.h"
#include "IO/ViewFileBuffer.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace StmData
	{
		class FileViewData : public IO::IStreamData
		{
		private:
			typedef struct
			{
				IO::ViewFileBuffer *file;
				UInt64 fileLength;
				UInt64 currentOffset;
				UTF8Char *fileName;
				UTF8Char *fullName;
				UInt8 *fptr;

				Sync::Mutex *mut;
				Int32 objectCnt;
			} FILEVIEWDATAHANDLE;

		private:
			FILEVIEWDATAHANDLE *fdh;
			UInt64 dataOffset;
			UInt64 dataLength;

		public:
			FileViewData(const FileViewData *fd, UInt64 offset, UInt64 length);
			FileViewData(const UTF8Char *fileName);
			virtual ~FileViewData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual const UTF8Char *GetFullName();
			virtual const UTF8Char *GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual OSInt GetSeekCount();

		private:
			void Close();
		};
	}
}
#endif
