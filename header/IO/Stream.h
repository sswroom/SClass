#ifndef _SM_IO_STREAM
#define _SM_IO_STREAM
#include "UnsafeArray.h"
#include "Data/ByteArray.h"
#include "IO/ParsedObject.h"
#include "IO/StreamData.h"
#include "Sync/Event.h"

namespace IO
{
	enum class StreamType
	{
		Unknown,
		SerialPort,
		USBxpress,
		TCPServer,
		TCPClient,
		File,
		HID,
		UDPServer,
		UDPClient,
		SSLClient,
		Memory,
		BufferedOutput,
		FTPClient,
		StreamData,
		Deflate,
		HTTPClient,
		TCPBoardcast,
		BufferedInput,
		StreamLogger,
		ProcessExecution,
		LZWDec,
		WebConnection,
		Hash,
		Inflate,
		WriteCache,
		LZWEnc,
		RS232GPIO,
		WindowsCOM,
		MemoryReading,
		DataCapture,
		WebSocket,
		Null,
		FileView,
		SSHTCPChannel
	};
	struct StreamReadReq;
	struct StreamWriteReq;

	class Stream : public IO::ParsedObject
	{
	public:
		Stream(NN<Text::String> sourceName);
		Stream(const Text::CStringNN &sourceName);
		virtual ~Stream(){};
		virtual Bool IsDown() const = 0;
		virtual UOSInt Read(const Data::ByteArray &buff) = 0;
		virtual UOSInt Write(Data::ByteArrayR buff) = 0;

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UOSInt EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UOSInt EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush() = 0;
		virtual void Close() = 0;
		virtual Bool CanSeek();
		virtual Bool Recover() = 0;
		virtual StreamType GetStreamType() const = 0;

		virtual IO::ParserType GetParserType() const;
		UInt64 ReadToEnd(NN<IO::Stream> stm, UOSInt buffSize);
		Bool WriteFromData(NN<IO::StreamData> data, UOSInt buffSize);
		UOSInt WriteCont(UnsafeArray<const UInt8> buff, UOSInt size);
	};

	Text::CStringNN StreamTypeGetName(StreamType st);
}
#endif
