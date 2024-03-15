#ifndef _SM_IO_DATACAPTURESTREAM
#define _SM_IO_DATACAPTURESTREAM
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class DataCaptureStream : public Stream
	{
	public:
		typedef void (__stdcall *DataHandler)(AnyType userObj, const UInt8 *dataBuff, UOSInt dataSize);
	private:
		IO::Stream *stm;
		DataHandler recvHdlr;
		DataHandler sendHdlr;
		AnyType hdlrObj;

	public:
		DataCaptureStream(IO::Stream *srcStream, DataHandler recvHdlr, DataHandler sendHdlr, AnyType hdlrObj);
		virtual ~DataCaptureStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
