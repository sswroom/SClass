#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Watchdog.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Optional<IO::Watchdog> wd;
Bool running;
Bool toStop;
Sync::Event *evt;

UInt32 __stdcall WatchdogThread(AnyType userObj)
{
	running = true;
	NN<IO::Watchdog> nnwd;
	if (wd.SetTo(nnwd))
	{
		while (!toStop)
		{
			nnwd->Keepalive();
			evt->Wait(1000);
		}
	}
	running = false;
	return 0;
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<IO::Watchdog> nnwd;
	IO::ConsoleWriter console;
	Int32 wdId;
	UTF8Char **argv;
	UOSInt argc;
	running = false;
	toStop = false;
	NEW_CLASS(evt, Sync::Event(true));

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
		if (nnwd->GetTimeoutSec(&timeoutSec))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Timeout = "));
			sb.AppendI64(timeoutSec);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Error in getting timeout value"));
		}

		if (nnwd->GetTemperature(&temp))
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

		if (nnwd->Enable())
		{
			console.WriteLine(CSTR("Watchdog enabled"));
			Sync::ThreadUtil::Create(WatchdogThread, 0);
			while (!running)
			{
				Sync::SimpleThread::Sleep(10);
			}
			console.WriteLine(CSTR("Running"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLine(CSTR("Exiting"));
			toStop = true;
			evt->Set();
			while (running)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
		else
		{
			console.WriteLine(CSTR("Error in enabling watchdog"));
		}
		nnwd.Delete();
	}
	DEL_CLASS(evt);
	return 0;
}

