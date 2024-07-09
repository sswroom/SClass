#ifndef _SM_IO_STMDATA_BUFFEREDSTREAMDATA
#define _SM_IO_STMDATA_BUFFEREDSTREAMDATA
#include "Data/ByteBuffer.h"
#include "IO/StreamData.h"

namespace IO
{
	namespace StmData
	{
		class BufferedStreamData : public IO::StreamData
		{
		private:
			NN<IO::StreamData> stmData;
			UInt64 buffOfst;
			UOSInt buffLength;
			Data::ByteBuffer buff;
			UInt64 dataLength;

		public:
			BufferedStreamData(NN<IO::StreamData> stmData);
			virtual ~BufferedStreamData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
			virtual NN<Text::String> GetFullName();
			virtual Text::CString GetShortName();
			virtual void SetFullName(Text::CStringNN fullName);
			virtual UInt64 GetDataSize();
			virtual UnsafeArrayOpt<const UInt8> GetPointer();

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual NN<Text::String> GetFullFileName();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();
		};
	}
}
#endif
