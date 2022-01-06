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
	UOSInt i;
	UOSInt j;
	console.WriteLineC(UTF8STRC("GPIO Pins:"));
	NEW_CLASS(ctrl, IO::GPIOControl());
	if (ctrl->IsError())
	{
		console.WriteLineC(UTF8STRC("Error"));
	}
	else
	{
		i = 0;
		j = ctrl->GetPinCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Pin "));
			sb.AppendUOSInt(i);
			sb.AppendC(UTF8STRC(" = "));
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
			console.WriteLineC(sb.ToString(), sb.GetLength());
			
			i++;
		}
	}
	DEL_CLASS(ctrl);
	return 0;
}
