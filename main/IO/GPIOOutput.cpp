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
	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 3)
	{
		if (Text::StrToInt32(argv[1], &pinNum) && Text::StrToInt32(argv[2], &val))
		{
			IO::GPIOControl gpio;
			if (gpio.SetPinState((UInt8)pinNum, val != 0))
			{
				console.WriteLine((const UTF8Char*)"GPIO set");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Error in setting GPIO");
			}			
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in parameter");
		}
	}
	else
	{
		console.Write((const UTF8Char*)"Usage: ");
		if (argc >= 1)
		{
			console.Write(argv[0]);
		}
		else
		{
			console.Write((const UTF8Char*)"GPIOOutput");
		}
		console.WriteLine((const UTF8Char*)" [Pin Num] [1/0]");
	}

	return 0;
}
