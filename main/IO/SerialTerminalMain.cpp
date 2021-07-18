#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

IO::ConsoleWriter *console;
IO::SerialPort *port;
Bool threadToStop;
Bool threadRunning;

UInt32 __stdcall RecvThread(void *userObj)
{
	UInt8 buff[512];
	UOSInt buffSize;
	threadRunning = true;
	while (!threadToStop)
	{
		buffSize = port->Read(buff, 512);
		if (buffSize > 0)
		{
			buff[buffSize] = 0;
			console->Write(buff, buffSize);
		}
	}
	threadRunning = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NEW_CLASS(console, IO::ConsoleWriter());

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);

	if (argc != 3)
	{
		console->Write((const UTF8Char*)"Usage: ");
		if (argc >= 1)
		{
			console->Write(argv[0]);
		}
		else
		{
			console->Write((const UTF8Char*)"SerialTerminal");
		}
		console->WriteLine((const UTF8Char*)" [PortNum] [BaudRate]");
	}
	else
	{
		UInt32 portNum;
		UInt32 baudRate;
		if (Text::StrToUInt32(argv[1], &portNum) && Text::StrToUInt32(argv[2], &baudRate))
		{
			NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
			if (port->IsError())
			{
				console->WriteLine((const UTF8Char*)"Error in opening serial port");
			}
			else
			{
				threadToStop = false;
				threadRunning = false;
				Sync::Thread::Create(RecvThread, 0);

				while ((sptr = IO::Console::GetLine(sbuff)) != 0)
				{
					if (Text::StrEquals(sbuff, (const UTF8Char*)"q"))
					{
						break;
					}
					sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
					port->Write(sbuff, (UOSInt)(sptr - sbuff));
				}
				threadToStop = true;
				port->Close();
				while (threadRunning)
				{
					Sync::Thread::Sleep(1);
				}
			}			
			DEL_CLASS(port);
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Parameter error");
		}
	}
	DEL_CLASS(console);
	return 0;
}
