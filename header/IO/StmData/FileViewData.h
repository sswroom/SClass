#ifndef _SM_IO_STMDATA_FILEVIEWDATA
#define _SM_IO_STMDATA_FILEVIEWDATA
#include "IO/StreamData.h"
#include "IO/ViewFileBuffer.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace StmData
	{
		class FileViewData : public IO::StreamData
		{
		private:
			struct FILEVIEWDATAHANDLE
			{
				IO::ViewFileBuffer *file;
				UInt64 fileLength;
				UInt64 currentOffset;
				Text::CString fileName;
				NotNullPtr<Text::String> fullName;
				UInt8 *fptr;

				Sync::Mutex mut;
				Int32 objectCnt;
			};

		private:
			FILEVIEWDATAHANDLE *fdh;
			UInt64 dataOffset;
			UInt64 dataLength;

		public:
			FileViewData(const FileViewData *fd, UInt64 offset, UInt64 length);
			FileViewData(const UTF8Char *fileName);
			virtual ~FileViewData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NotNullPtr<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual NotNullPtr<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

		private:
			void Close();
		};
	}
}
#endif
