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
		virtual UIntOS GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer) = 0;
		virtual NN<Text::String> GetFullName() const = 0;
		virtual Text::CString GetShortName() const = 0;
		virtual void SetFullName(Text::CStringNN fullName) {};
		virtual UInt64 GetDataSize() const = 0;
		virtual UnsafeArrayOpt<const UInt8> GetPointer() const = 0;

		virtual NN<StreamData> GetPartialData(UInt64 offset, UInt64 length) = 0;
		virtual Bool IsFullFile() const = 0;
		virtual NN<Text::String> GetFullFileName() const {return GetFullName();};
		virtual Bool IsLoading() const = 0;
		virtual UIntOS GetSeekCount() const = 0;
	};
}
#endif
