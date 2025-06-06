#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Watchdog.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Optional<IO::Watchdog> wd;
	NN<IO::Watchdog> nnwd;
	Int32 wdId;
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	UOSInt argc;
	argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2 && Text::StrToInt32(argv[1], wdId))
	{
		wd = IO::Watchdog::Create(wdId);
		if (wd.SetTo(nnwd) && nnwd->IsError())
		{
			wd.Delete();
		}
	}
	else
	{
		wd = IO::Watchdog::Create();
	}

	if (!wd.SetTo(nnwd))
	{
		console.WriteLine(CSTR("Watchdog not found"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Int32 timeoutSec;
		Double temp;
		if (nnwd->GetTimeoutSec(timeoutSec))
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

		if (nnwd->GetTemperature(temp))
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

		if (nnwd->Disable())
		{
			console.WriteLine(CSTR("Watchdog disabled"));
		}
		else
		{
			console.WriteLine(CSTR("Error in disabling watchdog"));
		}
		nnwd.Delete();
	}
	return 0;
}

