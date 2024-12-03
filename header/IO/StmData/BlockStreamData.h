#ifndef _SM_IO_STMDATA_BLOCKSTREAMDATA
#define _SM_IO_STMDATA_BLOCKSTREAMDATA
#include "IO/StreamData.h"
#include "IO/FileStream.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"

namespace IO
{
	namespace StmData
	{
		class BlockStreamData : public IO::StreamData
		{
		private:
			NN<IO::StreamData> sd;
			Data::ArrayListUInt64 dataOfstList;
			Data::ArrayListUInt64 stmOfstList;
			Data::ArrayListUInt32 lengthList;
			UInt64 totalSize;

		public:
			BlockStreamData(BlockStreamData &sd);
			BlockStreamData(NN<IO::StreamData> sd);
			virtual ~BlockStreamData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName() const;
			virtual Text::CString GetShortName() const;
			virtual UInt64 GetDataSize() const;
			virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile() const;
			virtual Bool IsLoading() const;
			virtual UOSInt GetSeekCount() const;

			void Append(UInt64 ofst, UInt32 length);
		};
	}
}
#endif
