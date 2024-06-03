#ifndef _SM_IO_STMDATA_MEMORYDATACOPY
#define _SM_IO_STMDATA_MEMORYDATACOPY
#include "IO/StreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryDataCopy : public IO::StreamData
		{
		private:
			typedef struct
			{
				UInt8 *data;
				UOSInt dataLength;
				Int32 useCnt;
				Text::String *fullName;
			} MemoryStats;
			MemoryStats *stat;
			const UInt8 *data;
			UOSInt dataLength;

			MemoryDataCopy(MemoryStats *stat, const UInt8 *data, UOSInt dataLength);
		public:
			MemoryDataCopy(const UInt8 *data, UOSInt dataLength);
			MemoryDataCopy(const Data::ByteArrayR &data);
			virtual ~MemoryDataCopy();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual void SetFullName(Text::CStringNN fullName);
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();
		};
	}
}
#endif
