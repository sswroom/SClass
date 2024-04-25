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
			virtual NN<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();

			void Append(UInt64 ofst, UInt32 length);
		};
	}
}
#endif
