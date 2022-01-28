#ifndef _SM_IO_ISTREAMDATA
#define _SM_IO_ISTREAMDATA
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class IStreamData
	{
	public:
		virtual ~IStreamData(){};
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer) = 0;
		virtual Text::String *GetFullName() = 0;
		virtual Text::CString GetShortName() = 0;
		virtual void SetFullName(const UTF8Char *fullName) {};
		virtual UInt64 GetDataSize() = 0;
		virtual const UInt8 *GetPointer() = 0;

		virtual IStreamData *GetPartialData(UInt64 offset, UInt64 length) = 0;
		virtual Bool IsFullFile() = 0;
		virtual Text::String *GetFullFileName() {return GetFullName();};
		virtual Bool IsLoading() = 0;
		virtual UOSInt GetSeekCount() = 0;
	};
}
#endif
