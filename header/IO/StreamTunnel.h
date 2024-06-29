#ifndef _SM_IO_STREAMTUNNEL
#define _SM_IO_STREAMTUNNEL
#include "IO/SeekableStream.h"
namespace IO
{
	class StreamTunnel : public IO::SeekableStream
	{
	private:
		IO::SeekableStream *sourceStm;
		IO::SeekableStream *readCapture;
		IO::SeekableStream *writeCapture;

	public:
		StreamTunnel(IO::SeekableStream *sourceStm, IO::SeekableStream *readCapture, IO::SeekableStream *writeCapture);
		virtual ~StreamTunnel();

		virtual Int32 Read(UInt8 *buff, Int32 size);
		virtual Int32 Write(const UInt8 *buff, Int32 size);

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UOSInt EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UOSInt EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Int64 Seek(SeekType origin, Int64 position);
		virtual Int64 GetPosition();
	};
}
#endif
