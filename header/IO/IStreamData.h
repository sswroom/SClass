#ifndef _SM_IO_ISTREAMDATA
#define _SM_IO_ISTREAMDATA

namespace IO
{
	class IStreamData
	{
	public:
		virtual ~IStreamData(){};
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer) = 0;
		virtual const UTF8Char *GetFullName() = 0;
		virtual const UTF8Char *GetShortName() = 0;
		virtual void SetFullName(const UTF8Char *fullName) {};
		virtual UInt64 GetDataSize() = 0;
		virtual const UInt8 *GetPointer() = 0;

		virtual IStreamData *GetPartialData(UInt64 offset, UInt64 length) = 0;
		virtual Bool IsFullFile() = 0;
		virtual const UTF8Char *GetFullFileName() {return GetFullName();};
		virtual Bool IsLoading() = 0;
		virtual OSInt GetSeekCount() = 0;
	};
};
#endif
