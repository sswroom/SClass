#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);

	if (argc != 2)
	{
		console.Write((const UTF8Char*)"Usage: ");
		if (argc >= 1)
		{
			console.Write(argv[0]);
		}
		else
		{
			console.Write((const UTF8Char*)"SerialReset");
		}
		console.WriteLine((const UTF8Char*)" [PortNum]");
	}
	else
	{
		UInt32 portNum;
		if (Text::StrToUInt32(argv[1], &portNum))
		{
			if (IO::SerialPort::ResetPort(portNum))
			{
				console.WriteLine((const UTF8Char*)"Serial port resetted");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Error in opening serial port");

			}			
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Parameter error");
		}
	}
	return 0;
}
