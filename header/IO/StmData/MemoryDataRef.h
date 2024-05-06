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
			Text::String *name;

		public:
			MemoryDataRef(UnsafeArray<const UInt8> data, UOSInt dataLength);
			MemoryDataRef(const Data::ByteArrayR &data);
			virtual ~MemoryDataRef();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual void SetFullName(Text::CString fullName);
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

			void SetName(Text::CStringNN name);
			void SetName(NN<Text::String> name);
		};
	}
}
#endif
