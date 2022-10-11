#ifndef _SM_IO_STMDATA_MEMORYDATACOPY
#define _SM_IO_STMDATA_MEMORYDATACOPY
#include "IO/IStreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryDataCopy : public IO::IStreamData
		{
		private:
			typedef struct
			{
				UInt8 *data;
				UOSInt dataLength;
				Int32 useCnt;
			} MemoryStats;
			MemoryStats *stat;
			const UInt8 *data;
			UOSInt dataLength;

			MemoryDataCopy(MemoryStats *stat, const UInt8 *data, UOSInt dataLength);
		public:
			MemoryDataCopy(const UInt8 *data, UOSInt dataLength);
			virtual ~MemoryDataCopy();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual Text::String *GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();
		};
	}
}
#endif
