#ifndef _SM_IO_STMDATA_MEMORYDATA2
#define _SM_IO_STMDATA_MEMORYDATA2
#include "IO/IStreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryData2 : public IO::IStreamData
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

			MemoryData2(MemoryStats *stat, const UInt8 *data, UOSInt dataLength);
		public:
			MemoryData2(const UInt8 *data, UOSInt dataLength);
			virtual ~MemoryData2();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual const UTF8Char *GetFullName();
			virtual const UTF8Char *GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::IStreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual OSInt GetSeekCount();
		};
	}
}
#endif
