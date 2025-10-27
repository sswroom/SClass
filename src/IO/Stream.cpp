#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/Stream.h"
#include "Text/StringBuilderUTF8.h"

IO::Stream::Stream(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
}

IO::Stream::Stream(const Text::CStringNN &sourceName) : IO::ParsedObject(sourceName)
{
}

Optional<IO::StreamReadReq> IO::Stream::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	UOSInt retVal = Read(buff);
	evt->Set();
	return (IO::StreamReadReq*)retVal;
}

UOSInt IO::Stream::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return (UOSInt)reqData.Ptr();
}

void IO::Stream::CancelRead(NN<IO::StreamReadReq> reqData)
{
}

Optional<IO::StreamWriteReq> IO::Stream::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	UOSInt retVal = Write(buff);
	evt->Set();
	return (IO::StreamWriteReq*)retVal;
}

UOSInt IO::Stream::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	return (UOSInt)reqData.Ptr();
}

void IO::Stream::CancelWrite(NN<IO::StreamWriteReq> reqData)
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

UInt64 IO::Stream::ReadToEnd(NN<IO::Stream> stm, UOSInt buffSize)
{
	UInt64 totalSize = 0;
	UOSInt readSize;
	UOSInt writeSize;
	Data::ByteBuffer buff(buffSize);
	while (true)
	{
		readSize = this->Read(buff);
		if (readSize <= 0)
		{
			break;
		}
		writeSize = stm->Write(buff.WithSize(readSize));
		totalSize += writeSize;
		if (readSize != writeSize)
		{
			break;
		}
	}
	return totalSize;
}

Optional<Text::String> IO::Stream::ReadAsString(UOSInt buffSize)
{
	Data::ByteBuffer buff(buffSize);
	UOSInt readSize;
	readSize = this->Read(buff);
	if (readSize <= 0)
		return 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(buff.Arr(), readSize);
	while (true)
	{
		readSize = this->Read(buff);
		if (readSize <= 0)
		{
			break;
		}
		sb.AppendC(buff.Arr(), readSize);
	}
	return Text::String::New(sb.ToCString());
}

Optional<Text::String> IO::Stream::ReadAsString()
{
	return this->ReadAsString(65536);
}

Bool IO::Stream::WriteFromData(NN<IO::StreamData> data, UOSInt buffSize)
{
	UInt64 currOfst = 0;
	UInt64 totalSize = 0;
	UOSInt readSize;
	UOSInt writeSize;
	Data::ByteBuffer buff(buffSize);
	while (true)
	{
		readSize = data->GetRealData(currOfst, buffSize, buff);
		if (readSize <= 0)
		{
			break;
		}
		currOfst += readSize;
		writeSize = this->Write(buff.WithSize(readSize));
		totalSize += writeSize;
		if (readSize != writeSize)
		{
			break;
		}
	}
	return totalSize == data->GetDataSize();
}

UOSInt IO::Stream::WriteCont(UnsafeArray<const UInt8> buff, UOSInt size)
{
	UOSInt totalWrite = 0;
	UOSInt writeSize;
	while (totalWrite < size)
	{
		writeSize = this->Write(Data::ByteArrayR(&buff[totalWrite], size - totalWrite));
		if (writeSize == 0)
			return totalWrite;
		totalWrite += writeSize;
	}
	return totalWrite;
}

Text::CStringNN IO::StreamTypeGetName(StreamType st)
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
	case StreamType::DeflateStream:
		return CSTR("DeflateStream");
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
	case StreamType::InflateStream:
		return CSTR("InflateStream");
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
	case StreamType::Null:
		return CSTR("Null");
	case StreamType::FileView:
		return CSTR("FileView");
	case StreamType::SSHTCPChannel:
		return CSTR("SSHTCPChannel");
	case StreamType::Inflater:
		return CSTR("Inflater");
	case StreamType::Deflater:
		return CSTR("Deflater");
	case StreamType::GSMMuxPort:
		return CSTR("GSMMuxPort");
	case StreamType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
