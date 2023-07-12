#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
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
				sb.AppendUTF8Char('1');
			}
			else
			{
				sb.AppendUTF8Char('0');
			}
			sb.AppendUTF8Char(',');
			sb.Append(IO::GPIOControl::PinModeGetName(i, ctrl->GetPinMode(i)));
			console.WriteLineC(sb.ToString(), sb.GetLength());
			
			i++;
		}
	}
	DEL_CLASS(ctrl);
	return 0;
}
