#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Watchdog.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::Watchdog *wd;
	Int32 wdId;
	UTF8Char **argv;
	UOSInt argc;
	argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2 && Text::StrToInt32(argv[1], wdId))
	{
		wd = IO::Watchdog::Create(wdId);
		if (wd && wd->IsError())
		{
			DEL_CLASS(wd);
			wd = 0;
		}
	}
	else
	{
		wd = IO::Watchdog::Create();
	}

	if (wd == 0)
	{
		console.WriteLine(CSTR("Watchdog not found"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Int32 timeoutSec;
		Double temp;
		if (wd->GetTimeoutSec(&timeoutSec))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Timeout = "));
			sb.AppendI32(timeoutSec);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Error in getting timeout value"));
		}

		if (wd->GetTemperature(&temp))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Temperature = "));
			sb.AppendDouble(temp);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Error in getting temperature value"));
		}

		if (wd->Disable())
		{
			console.WriteLine(CSTR("Watchdog disabled"));
		}
		else
		{
			console.WriteLine(CSTR("Error in disabling watchdog"));
		}
		DEL_CLASS(wd);
	}
	return 0;
}

