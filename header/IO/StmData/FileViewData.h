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
				Text::CStringNN fileName;
				NN<Text::String> fullName;
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
			virtual NN<Text::String> GetFullName() const;
			virtual Text::CString GetShortName() const;
			virtual UInt64 GetDataSize() const;
			virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile() const;
			virtual Bool IsLoading() const;
			virtual UOSInt GetSeekCount() const;

		private:
			void Close();
		};
	}
}
#endif
