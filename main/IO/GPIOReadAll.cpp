#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOControl *ctrl;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	UInt8 i;
	UInt8 j;
	console.WriteLine((const UTF8Char*)"GPIO Pins:");
	NEW_CLASS(ctrl, IO::GPIOControl());
	if (ctrl->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error");
	}
	else
	{
		i = 0;
		j = ctrl->GetPinCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Pin ");
			sb.AppendU16(i);
			sb.Append((const UTF8Char*)" = ");
			if (ctrl->IsPinHigh(i))
			{
				sb.AppendChar('1', 1);
			}
			else
			{
				sb.AppendChar('0', 1);
			}
			sb.AppendChar(',', 1);
			sb.Append(IO::GPIOControl::PinModeGetName(i, ctrl->GetPinMode(i)));
			console.WriteLine(sb.ToString());
			
			i++;
		}
	}
	DEL_CLASS(ctrl);
	return 0;
}
