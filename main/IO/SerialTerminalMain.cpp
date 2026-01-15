#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

IO::ConsoleWriter *console;
IO::SerialPort *port;
Bool threadToStop;
Bool threadRunning;

UInt32 __stdcall RecvThread(AnyType userObj)
{
	UInt8 buff[512];
	UIntOS buffSize;
	threadRunning = true;
	while (!threadToStop)
	{
		buffSize = port->Read(BYTEARR(buff));
		if (buffSize > 0)
		{
			buff[buffSize] = 0;
			console->Write(Text::CStringNN(buff, buffSize));
		}
	}
	threadRunning = false;
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NEW_CLASS(console, IO::ConsoleWriter());

	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);

	if (argc != 3)
	{
		console->Write(CSTR("Usage: "));
		if (argc >= 1)
		{
			console->Write(Text::CStringNN::FromPtr(argv[0]));
		}
		else
		{
			console->Write(CSTR("SerialTerminal"));
		}
		console->WriteLine(CSTR(" [PortNum] [BaudRate]"));
	}
	else
	{
		UInt32 portNum;
		UInt32 baudRate;
		if (Text::StrToUInt32(argv[1], portNum) && Text::StrToUInt32(argv[2], baudRate))
		{
			NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
			if (port->IsError())
			{
				console->WriteLine(CSTR("Error in opening serial port"));
			}
			else
			{
				threadToStop = false;
				threadRunning = false;
				Sync::ThreadUtil::Create(RecvThread, 0);

				while (true)
				{
					sptr = IO::Console::GetLine(sbuff);
					if (Text::StrEquals(sbuff, (const UTF8Char*)"q"))
					{
						break;
					}
					sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
					port->Write(Data::ByteArrayR(sbuff, (UIntOS)(sptr - sbuff)));
				}
				threadToStop = true;
				port->Close();
				while (threadRunning)
				{
					Sync::SimpleThread::Sleep(1);
				}
			}			
			DEL_CLASS(port);
		}
		else
		{
			console->WriteLine(CSTR("Parameter error"));
		}
	}
	DEL_CLASS(console);
	return 0;
}
