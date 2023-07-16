#ifndef _SM_IO_STMDATA_MEMORYDATAREF
#define _SM_IO_STMDATA_MEMORYDATAREF
#include "Data/ByteArray.h"
#include "IO/StreamData.h"

namespace IO
{
	namespace StmData
	{
		class MemoryDataRef : public IO::StreamData
		{
		private:
			Data::ByteArrayR data;

		public:
			MemoryDataRef(const UInt8 *data, UOSInt dataLength);
			MemoryDataRef(const Data::ByteArrayR &data);
			virtual ~MemoryDataRef();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NotNullPtr<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::StreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();
		};
	}
}
#endif
