#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Watchdog.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::Watchdog *wd;
	Int32 wdId;
	UTF8Char **argv;
	UOSInt argc;
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2 && Text::StrToInt32(argv[1], &wdId))
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
		console.WriteLine((const UTF8Char*)"Watchdog not found");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Int32 timeoutSec;
		Double temp;
		if (wd->GetTimeoutSec(&timeoutSec))
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Timeout = ");
			sb.AppendI32(timeoutSec);
			console.WriteLine(sb.ToString());
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in getting timeout value");
		}

		if (wd->GetTemperature(&temp))
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Temperature = ");
			Text::SBAppendF64(&sb, temp);
			console.WriteLine(sb.ToString());
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in getting temperature value");
		}

		if (wd->Disable())
		{
			console.WriteLine((const UTF8Char*)"Watchdog disabled");
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in disabling watchdog");
		}
		DEL_CLASS(wd);
	}
	return 0;
}

