#ifndef _SM_IO_STREAMLOGGER
#define _SM_IO_STREAMLOGGER
#include "IO/FileStream.h"

namespace IO
{
	class StreamLogger : public IO::Stream
	{
	private:
		typedef struct
		{
			UnsafeArray<const UInt8> buff;
			void *reqData;
		} MyReqData;
	private:
		NN<IO::Stream> stm;
		Bool needRelease;
		IO::FileStream *writeLog;
		IO::FileStream *readLog;
	public:
		StreamLogger(NN<IO::Stream> srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath);
		virtual ~StreamLogger();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(UnsafeArray<const UInt8> buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
