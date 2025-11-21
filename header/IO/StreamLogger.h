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
			NN<IO::StreamWriteReq> reqData;
		} MyReqData;
	private:
		NN<IO::Stream> stm;
		Bool needRelease;
		Optional<IO::FileStream> writeLog;
		Optional<IO::FileStream> readLog;
	public:
		StreamLogger(NN<IO::Stream> srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath);
		virtual ~StreamLogger();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UOSInt EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UOSInt EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
