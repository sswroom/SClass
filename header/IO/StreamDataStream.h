#ifndef _SM_IO_STREAMDATASTREAM
#define _SM_IO_STREAMDATASTREAM
#include "IO/StreamData.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class StreamDataStream : public IO::SeekableStream
	{
	private:
		NN<IO::StreamData> data;
		UInt64 currOfst;
		UInt64 stmDataLeng;

	public:
		StreamDataStream(NN<IO::StreamData> data);
		StreamDataStream(NN<IO::StreamData> data, UInt64 ofst, UInt64 length);
		virtual ~StreamDataStream();

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
		virtual StreamType GetStreamType() const;
		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
	};
}
#endif
