#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Int32 pinNum;
	Int32 val;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 3)
	{
		if (Text::StrToInt32(argv[1], &pinNum) && Text::StrToInt32(argv[2], &val))
		{
			IO::GPIOControl gpio;
			if (gpio.SetPinState((UInt8)pinNum, val != 0))
			{
				console.WriteLineC(UTF8STRC("GPIO set"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in setting GPIO"));
			}			
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in parameter"));
		}
	}
	else
	{
		console.WriteStrC(UTF8STRC("Usage: "));
		if (argc >= 1)
		{
			console.WriteStr(Text::CString::FromPtr(argv[0]));
		}
		else
		{
			console.WriteStrC(UTF8STRC("GPIOOutput"));
		}
		console.WriteLineC(UTF8STRC(" [Pin Num] [1/0]"));
	}

	return 0;
}
