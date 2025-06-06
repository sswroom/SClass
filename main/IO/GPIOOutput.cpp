#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Int32 pinNum;
	Int32 val;
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 3)
	{
		if (Text::StrToInt32(argv[1], pinNum) && Text::StrToInt32(argv[2], val))
		{
			IO::GPIOControl gpio;
			if (gpio.SetPinState((UInt8)pinNum, val != 0))
			{
				console.WriteLine(CSTR("GPIO set"));
			}
			else
			{
				console.WriteLine(CSTR("Error in setting GPIO"));
			}			
		}
		else
		{
			console.WriteLine(CSTR("Error in parameter"));
		}
	}
	else
	{
		console.Write(CSTR("Usage: "));
		if (argc >= 1)
		{
			console.Write(Text::CStringNN::FromPtr(argv[0]));
		}
		else
		{
			console.Write(CSTR("GPIOOutput"));
		}
		console.WriteLine(CSTR(" [Pin Num] [1/0]"));
	}

	return 0;
}
