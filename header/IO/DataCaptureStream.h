#ifndef _SM_IO_DATACAPTURESTREAM
#define _SM_IO_DATACAPTURESTREAM
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class DataCaptureStream : public Stream
	{
	public:
		typedef void (CALLBACKFUNC DataHandler)(AnyType userObj, Data::ByteArrayR dataBuff);
	private:
		NN<IO::Stream> stm;
		DataHandler recvHdlr;
		DataHandler sendHdlr;
		AnyType hdlrObj;

	public:
		DataCaptureStream(NN<IO::Stream> srcStream, DataHandler recvHdlr, DataHandler sendHdlr, AnyType hdlrObj);
		virtual ~DataCaptureStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
