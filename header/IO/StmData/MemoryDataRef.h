#ifndef _SM_IO_STMDATA_MEMORYDATAREF
#define _SM_IO_STMDATA_MEMORYDATAREF
#include "IO/IStreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryDataRef : public IO::IStreamData
		{
		private:
			const UInt8 *data;
			UOSInt dataLength;

		public:
			MemoryDataRef(const UInt8 *data, UOSInt dataLength);
			virtual ~MemoryDataRef();

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
