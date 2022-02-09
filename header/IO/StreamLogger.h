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
			const UInt8 *buff;
			void *reqData;
		} MyReqData;
	private:
		IO::Stream *stm;
		Bool needRelease;
		IO::FileStream *writeLog;
		IO::FileStream *readLog;
	public:
		StreamLogger(IO::Stream *srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath);
		virtual ~StreamLogger();

		virtual Bool IsDown();
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
