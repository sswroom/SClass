#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/SerialPort.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);

	if (argc != 2)
	{
		console.Write(CSTR("Usage: "));
		if (argc >= 1)
		{
			console.Write(Text::CStringNN::FromPtr(argv[0]));
		}
		else
		{
			console.Write(CSTR("SerialReset"));
		}
		console.WriteLine(CSTR(" [PortNum]"));
	}
	else
	{
		UInt32 portNum;
		if (Text::StrToUInt32(argv[1], portNum))
		{
			if (IO::SerialPort::ResetPort(portNum))
			{
				console.WriteLine(CSTR("Serial port resetted"));
			}
			else
			{
				console.WriteLine(CSTR("Error in opening serial port"));

			}			
		}
		else
		{
			console.WriteLine(CSTR("Parameter error"));
		}
	}
	return 0;
}
