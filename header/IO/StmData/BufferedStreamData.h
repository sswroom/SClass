#ifndef _SM_IO_STMDATA_BUFFEREDSTREAMDATA
#define _SM_IO_STMDATA_BUFFEREDSTREAMDATA
#include "IO/StreamData.h"

namespace IO
{
	namespace StmData
	{
		class BufferedStreamData : public IO::StreamData
		{
		private:
			IO::StreamData *stmData;
			UInt64 buffOfst;
			UOSInt buffLength;
			UInt8 *buff;
			UInt64 dataLength;

		public:
			BufferedStreamData(IO::StreamData *stmData);
			virtual ~BufferedStreamData();

			virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
			virtual Text::String *GetFullName();
			virtual Text::CString GetShortName();
			virtual void SetFullName(Text::CString fullName);
			virtual UInt64 GetDataSize();
			virtual const UInt8 *GetPointer();

			virtual IO::StreamData *GetPartialData(UInt64 offset, UInt64 length);
			virtual Bool IsFullFile();
			virtual Text::String *GetFullFileName();
			virtual Bool IsLoading();
			virtual UOSInt GetSeekCount();
		};
	}
}
#endif
