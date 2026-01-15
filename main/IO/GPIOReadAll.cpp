#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::GPIOControl *ctrl;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	UIntOS i;
	UIntOS j;
	console.WriteLine(CSTR("GPIO Pins:"));
	NEW_CLASS(ctrl, IO::GPIOControl());
	if (ctrl->IsError())
	{
		console.WriteLine(CSTR("Error"));
	}
	else
	{
		i = 0;
		j = ctrl->GetPinCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Pin "));
			sb.AppendUIntOS(i);
			sb.AppendC(UTF8STRC(" = "));
			if (ctrl->IsPinHigh(i))
			{
				sb.AppendUTF8Char('1');
			}
			else
			{
				sb.AppendUTF8Char('0');
			}
			sb.AppendUTF8Char(',');
			sb.Append(IO::GPIOControl::PinModeGetName(i, ctrl->GetPinMode(i)));
			console.WriteLine(sb.ToCString());
			
			i++;
		}
	}
	DEL_CLASS(ctrl);
	return 0;
}
