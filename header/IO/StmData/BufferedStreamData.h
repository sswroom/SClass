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
			virtual NN<Text::String> GetFullName() const;
			virtual Text::CString GetShortName() const;
			virtual void SetFullName(Text::CStringNN fullName);
			virtual UInt64 GetDataSize() const;
			virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

			virtual NN<IO::StreamData> GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile() const;
			virtual NN<Text::String> GetFullFileName() const;
			virtual Bool IsLoading() const;
			virtual UOSInt GetSeekCount() const;
		};
	}
}
#endif
