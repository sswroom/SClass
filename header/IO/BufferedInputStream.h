#ifndef _SM_IO_BUFFEREDINPUTSTREAM
#define _SM_IO_BUFFEREDINPUTSTREAM
#include "Data/ByteBuffer.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class BufferedInputStream : public SeekableStream
	{
	private:
		NN<IO::SeekableStream> stm;
		Data::ByteBuffer buff;
		UIntOS buffOfst;
		UInt64 stmPos;
		UIntOS stmBuffSize;

	public:
		BufferedInputStream(NN<IO::SeekableStream> stm, UIntOS buffSize);
		virtual ~BufferedInputStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;

		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();
	};
}
#endif
