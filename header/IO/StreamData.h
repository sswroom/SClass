#ifndef _SM_IO_STREAMDATA
#define _SM_IO_STREAMDATA
#include "Data/ByteArray.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class StreamData
	{
	public:
		virtual ~StreamData(){};
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer) = 0;
		virtual NotNullPtr<Text::String> GetFullName() = 0;
		virtual Text::CString GetShortName() = 0;
		virtual void SetFullName(Text::CString fullName) {};
		virtual UInt64 GetDataSize() = 0;
		virtual const UInt8 *GetPointer() = 0;

		virtual StreamData *GetPartialData(UInt64 offset, UInt64 length) = 0;
		virtual Bool IsFullFile() = 0;
		virtual NotNullPtr<Text::String> GetFullFileName() {return GetFullName();};
		virtual Bool IsLoading() = 0;
		virtual UOSInt GetSeekCount() = 0;
	};
}
#endif
