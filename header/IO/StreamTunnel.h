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

		virtual void *BeginRead(UInt8 *buff, Int32 size, Sync::Event *evt);
		virtual Int32 EndRead(void *reqData, Bool toWait);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, Int32 size, Sync::Event *evt);
		virtual Int32 EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Int64 Seek(SeekType origin, Int64 position);
		virtual Int64 GetPosition();
	};
}
#endif
