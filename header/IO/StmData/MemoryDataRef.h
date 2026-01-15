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
			Optional<Text::String> name;

		public:
			MemoryDataRef(UnsafeArray<const UInt8> data, UIntOS dataLength);
			MemoryDataRef(const Data::ByteArrayR &data);
			virtual ~MemoryDataRef();

			virtual UIntOS GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName() const;
			virtual Text::CString GetShortName() const;
			virtual void SetFullName(Text::CStringNN fullName);
			virtual UInt64 GetDataSize() const;
			virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile() const;
			virtual Bool IsLoading() const;
			virtual UIntOS GetSeekCount() const;

			void SetName(Text::CStringNN name);
			void SetName(NN<Text::String> name);
		};
	}
}
#endif
