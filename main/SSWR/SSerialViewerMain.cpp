#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
IO::SerialPort *port;
Bool readError;
Bool toStop;
Bool running;

static UInt32 __stdcall SerialViewer(AnyType userObj)
{
	UInt8 readBuff[1024];
	UOSInt readSize;
	{
		running = true;
		Text::StringBuilderUTF8 sb;
		while (!toStop)
		{
			readSize = port->Read(BYTEARR(readBuff));
			if (readSize <= 0)
			{
				readError = true;
				break;
			}
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp::Now());
			console->WriteLine(sb.ToCString());
			sb.ClearStr();
			sb.AppendHexBuff(readBuff, readSize, ' ', Text::LineBreakType::CRLF);
			console->WriteLine(sb.ToCString());
			console->WriteLine();
		}
	}
	running = false;
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	UInt32 baudRate = 115200;
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc <= 1)
	{
		console->WriteLine(CSTR("Usage: SSwerialViewer [portNo] [baudRate]"));
	}
	else
	{
		UInt32 portNo = 0;
		Text::StrToUInt32(argv[1], portNo);
		if (portNo == 0)
		{
			console->WriteLine(CSTR("PortNo is not correct"));
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (argc >= 3)
			{
				Text::StrToUInt32(argv[2], baudRate);
			}
			running = false;
			readError = false;
			toStop = false;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Opening Serial Port "));
			sb.AppendU32(portNo);
			sb.AppendC(UTF8STRC(" with baudrate = "));
			sb.AppendU32(baudRate);
			NEW_CLASS(port, IO::SerialPort(portNo, baudRate, IO::SerialPort::PARITY_NONE, false));
			if (port->IsError())
			{
				console->WriteLine(CSTR("Error in opening serial port"));
			}
			else
			{
				console->WriteLine(CSTR("Running"));
				Sync::ThreadUtil::Create(SerialViewer, 0);
				progCtrl->WaitForExit(progCtrl);
				console->WriteLine(CSTR("Exiting"));
				port->Close();
				while (running)
				{
					Sync::SimpleThread::Sleep(1);
				}
			}
			DEL_CLASS(port);
		}
	}

	DEL_CLASS(console);
	return 0;
}
