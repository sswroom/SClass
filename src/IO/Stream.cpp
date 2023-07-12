#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Stream.h"

IO::Stream::Stream(NotNullPtr<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

IO::Stream::Stream(Text::CString sourceName) : IO::ParsedObject(sourceName)
{
}

void *IO::Stream::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt retVal = Read(buff, size);
	evt->Set();
	return (void*)retVal;
}

UOSInt IO::Stream::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return (UOSInt)reqData;
}

void IO::Stream::CancelRead(void *reqData)
{
}

void *IO::Stream::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt retVal = Write(buff, size);
	evt->Set();
	return (void*)retVal;
}

UOSInt IO::Stream::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void IO::Stream::CancelWrite(void *reqData)
{
}

Bool IO::Stream::CanSeek()
{
	return false;
}

IO::ParserType IO::Stream::GetParserType() const
{
	return IO::ParserType::Stream;
}

UInt64 IO::Stream::ReadToEnd(IO::Stream *stm, UOSInt buffSize)
{
	UInt64 totalSize = 0;
	UOSInt readSize;
	UOSInt writeSize;
	UInt8 *buff = MemAlloc(UInt8, buffSize);
	while (true)
	{
		readSize = this->Read(buff, buffSize);
		if (readSize <= 0)
		{
			break;
		}
		writeSize = stm->Write(buff, readSize);
		totalSize += writeSize;
		if (readSize != writeSize)
		{
			break;
		}
	}
	MemFree(buff);
	return totalSize;
}

Bool IO::Stream::WriteFromData(IO::StreamData *data, UOSInt buffSize)
{
	UInt64 currOfst = 0;
	UInt64 totalSize = 0;
	UOSInt readSize;
	UOSInt writeSize;
	UInt8 *buff = MemAlloc(UInt8, buffSize);
	while (true)
	{
		readSize = data->GetRealData(currOfst, buffSize, buff);
		if (readSize <= 0)
		{
			break;
		}
		currOfst += readSize;
		writeSize = this->Write(buff, readSize);
		totalSize += writeSize;
		if (readSize != writeSize)
		{
			break;
		}
	}
	MemFree(buff);
	return totalSize == data->GetDataSize();
}

Text::CString IO::StreamTypeGetName(StreamType st)
{
	switch (st)
	{
	case StreamType::SerialPort:
		return CSTR("Serial Port");
	case StreamType::USBxpress:
		return CSTR("Silicon Laboratories USBXpress");
	case StreamType::TCPServer:
		return CSTR("TCP Server");
	case StreamType::TCPClient:
		return CSTR("TCP Client");
	case StreamType::File:
		return CSTR("File (Readonly)");
	case StreamType::HID:
		return CSTR("HID");
	case StreamType::UDPServer:
		return CSTR("UDP Server");
	case StreamType::UDPClient:
		return CSTR("UDP Client");
	case StreamType::SSLClient:
		return CSTR("SSL Client");
	case StreamType::Memory:
		return CSTR("Memory");
	case StreamType::BufferedOutput:
		return CSTR("BufferedOutput");
	case StreamType::FTPClient:
		return CSTR("FTP Client");
	case StreamType::StreamData:
		return CSTR("StreamData");
	case StreamType::Deflate:
		return CSTR("Deflate");
	case StreamType::HTTPClient:
		return CSTR("HTTP Client");
	case StreamType::TCPBoardcast:
		return CSTR("TCP Boardcast");
	case StreamType::BufferedInput:
		return CSTR("BufferedInput");
	case StreamType::StreamLogger:
		return CSTR("Stream Logger");
	case StreamType::ProcessExecution:
		return CSTR("Process Execution");
	case StreamType::LZWDec:
		return CSTR("LZWDec");
	case StreamType::WebConnection:
		return CSTR("WebConnection");
	case StreamType::Hash:
		return CSTR("Hash");
	case StreamType::Inflate:
		return CSTR("Inflate");
	case StreamType::WriteCache:
		return CSTR("WriteCache");
	case StreamType::LZWEnc:
		return CSTR("LZWEnc");
	case StreamType::RS232GPIO:
		return CSTR("RS232 GPIO");
	case StreamType::WindowsCOM:
		return CSTR("Windows COM");
	case StreamType::MemoryReading:
		return CSTR("MemoryReading");
	case StreamType::DataCapture:
		return CSTR("DataCapture");
	case StreamType::WebSocket:
		return CSTR("WebSocket");
	case StreamType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
