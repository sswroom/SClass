#ifndef _SM_IO_STREAM
#define _SM_IO_STREAM
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
		Null
	};


	class Stream : public IO::ParsedObject
	{
	public:
		Stream(NotNullPtr<Text::String> sourceName);
		Stream(const Text::CString &sourceName);
		virtual ~Stream(){};
		virtual Bool IsDown() const = 0;
		virtual UOSInt Read(const Data::ByteArray &buff) = 0;
		virtual UOSInt Write(const UInt8 *buff, UOSInt size) = 0;

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush() = 0;
		virtual void Close() = 0;
		virtual Bool CanSeek();
		virtual Bool Recover() = 0;
		virtual StreamType GetStreamType() const = 0;

		virtual IO::ParserType GetParserType() const;
		UInt64 ReadToEnd(IO::Stream *stm, UOSInt buffSize);
		Bool WriteFromData(NotNullPtr<IO::StreamData> data, UOSInt buffSize);
	};

	Text::CString StreamTypeGetName(StreamType st);
}
#endif
