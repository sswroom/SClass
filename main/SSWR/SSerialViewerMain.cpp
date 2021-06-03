#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
IO::SerialPort *port;
Bool readError;
Bool toStop;
Bool running;

static UInt32 __stdcall SerialViewer(void *userObj)
{
	UInt8 readBuff[1024];
	UOSInt readSize;
	Text::StringBuilderUTF8 *sb;
	Data::DateTime *dt;
	running = true;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(dt, Data::DateTime());
	while (!toStop)
	{
		readSize = port->Read(readBuff, 1024);
		if (readSize <= 0)
		{
			readError = true;
			break;
		}
		dt->SetCurrTime();
		sb->ClearStr();
		sb->AppendDate(dt);
		console->WriteLine(sb->ToString());
		sb->ClearStr();
		sb->AppendHex(readBuff, readSize, ' ', Text::LBT_CRLF);
		console->WriteLine(sb->ToString());
		console->WriteLine();
	}
	DEL_CLASS(dt);
	DEL_CLASS(sb);
	running = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt argc;
	UTF8Char **argv;
	UInt32 baudRate = 115200;
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc <= 1)
	{
		console->WriteLine((const UTF8Char*)"Usage: SSwerialViewer [portNo] [baudRate]");
	}
	else
	{
		UInt32 portNo = 0;
		Text::StrToUInt32(argv[1], &portNo);
		if (portNo == 0)
		{
			console->WriteLine((const UTF8Char*)"PortNo is not correct");
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (argc >= 3)
			{
				Text::StrToUInt32(argv[2], &baudRate);
			}
			running = false;
			readError = false;
			toStop = false;
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Opening Serial Port ");
			sb.AppendU32(portNo);
			sb.Append((const UTF8Char*)" with baudrate = ");
			sb.AppendU32(baudRate);
			NEW_CLASS(port, IO::SerialPort(portNo, baudRate, IO::SerialPort::PARITY_NONE, false));
			if (port->IsError())
			{
				console->WriteLine((const UTF8Char*)"Error in opening serial port");
			}
			else
			{
				console->WriteLine((const UTF8Char*)"Running");
				Sync::Thread::Create(SerialViewer, 0);
				progCtrl->WaitForExit(progCtrl);
				console->WriteLine((const UTF8Char*)"Exiting");
				port->Close();
				while (running)
				{
					Sync::Thread::Sleep(1);
				}
			}
			DEL_CLASS(port);
		}
	}

	DEL_CLASS(console);
	return 0;
}
