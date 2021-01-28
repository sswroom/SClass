#ifndef _SM_IO_STMDATA_MEMORYDATA
#define _SM_IO_STMDATA_MEMORYDATA
#include "IO/IStreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryData : public IO::IStreamData
		{
		private:
			const UInt8 *data;
			UOSInt dataLength;

		public:
			MemoryData(const UInt8 *data, UOSInt dataLength);
			virtual ~MemoryData();

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
