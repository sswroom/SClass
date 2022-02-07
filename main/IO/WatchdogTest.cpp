#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Watchdog.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::Watchdog *wd;
Bool running;
Bool toStop;
Sync::Event *evt;

UInt32 __stdcall WatchdogThread(void *userObj)
{
	running = true;
	while (!toStop)
	{
		wd->Keepalive();
		evt->Wait(1000);
	}
	running = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Int32 wdId;
	UTF8Char **argv;
	UOSInt argc;
	running = false;
	toStop = false;
	NEW_CLASS(evt, Sync::Event(true));

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
		console.WriteLineC(UTF8STRC("Watchdog not found"));
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
			sb.AppendI64(timeoutSec);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in getting timeout value"));
		}

		if (wd->GetTemperature(&temp))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Temperature = "));
			Text::SBAppendF64(&sb, temp);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in getting temperature value"));
		}

		if (wd->Enable())
		{
			console.WriteLineC(UTF8STRC("Watchdog enabled"));
			Sync::Thread::Create(WatchdogThread, 0);
			while (!running)
			{
				Sync::Thread::Sleep(10);
			}
			console.WriteLineC(UTF8STRC("Running"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLineC(UTF8STRC("Exiting"));
			toStop = true;
			evt->Set();
			while (running)
			{
				Sync::Thread::Sleep(10);
			}
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in enabling watchdog"));
		}
		DEL_CLASS(wd);
	}
	DEL_CLASS(evt);
	return 0;
}

