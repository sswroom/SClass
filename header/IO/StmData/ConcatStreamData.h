#ifndef _SM_IO_STMDATA_CONCATSTREAMDATA
#define _SM_IO_STMDATA_CONCATSTREAMDATA
#include "Data/ArrayListNN.h"
#include "Data/ArrayListUInt64.h"
#include "IO/StreamData.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace StmData
	{
		class ConcatStreamData : public IO::StreamData
		{
		private:
			struct CONCATDATABASE
			{
				Data::ArrayListNN<IO::StreamData> dataList;
				Data::ArrayListUInt64 ofstList;

				UInt64 totalSize;
				NN<Text::String> fileName;
				Sync::Mutex mut;
				UInt32 objectCnt;
			};


		private:
			CONCATDATABASE *cdb;
			UInt64 dataOffset;
			UInt64 dataLength;

			ConcatStreamData(CONCATDATABASE *cdb, UInt64 dataOffset, UInt64 dataLength);
		public:
			ConcatStreamData(NN<Text::String> fileName);
			ConcatStreamData(Text::CString fileName);
			virtual ~ConcatStreamData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

			void AddData(NN<IO::StreamData> data);
		};
	}
}
#endif
