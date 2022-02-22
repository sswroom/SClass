#ifndef _SM_IO_STMDATA_CONCATSTREAMDATA
#define _SM_IO_STMDATA_CONCATSTREAMDATA
#include "Data/ArrayListUInt64.h"
#include "IO/IStreamData.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace StmData
	{
		class ConcatStreamData : public IO::IStreamData
		{
		private:
			typedef struct
			{
				Data::ArrayList<IO::IStreamData *> *dataList;
				Data::ArrayListUInt64 *ofstList;

				UInt64 totalSize;
				Text::String *fileName;
				Sync::Mutex *mut;
				UInt32 objectCnt;
			} CONCATDATABASE;


		private:
			CONCATDATABASE *cdb;
			UInt64 dataOffset;
			UInt64 dataLength;

			ConcatStreamData(CONCATDATABASE *cdb, UInt64 dataOffset, UInt64 dataLength);
		public:
			ConcatStreamData(Text::String *fileName);
			ConcatStreamData(Text::CString fileName);
			virtual ~ConcatStreamData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual Text::String *GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

			void AddData(IO::IStreamData *data);
		};
	}
}
#endif
