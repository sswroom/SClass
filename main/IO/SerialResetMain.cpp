#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);

	if (argc != 2)
	{
		console.WriteStrC(UTF8STRC("Usage: "));
		if (argc >= 1)
		{
			console.WriteStr(Text::CString::FromPtr(argv[0]));
		}
		else
		{
			console.WriteStrC(UTF8STRC("SerialReset"));
		}
		console.WriteLineC(UTF8STRC(" [PortNum]"));
	}
	else
	{
		UInt32 portNum;
		if (Text::StrToUInt32(argv[1], portNum))
		{
			if (IO::SerialPort::ResetPort(portNum))
			{
				console.WriteLineC(UTF8STRC("Serial port resetted"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in opening serial port"));

			}			
		}
		else
		{
			console.WriteLineC(UTF8STRC("Parameter error"));
		}
	}
	return 0;
}
