#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/IniFile.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"

IO::GPIOPin *pin;
Sync::Event *evt;
Bool threadRunning;
Bool threadToStop;

UInt32 __stdcall ThreadFunc(void *userObj)
{
	threadRunning = true;
	while (!threadToStop)
	{
		pin->SetPinState(true);
		evt->Wait(500);
		if (!threadToStop)
		{
			pin->SetPinState(false);
			evt->Wait(500);
		}

	}
	threadRunning = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::GPIOControl *gpioCtrl;
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	const UTF8Char *csptr;
	UInt16 pinNum;
	if (cfg)
	{
		pinNum = 0;
		csptr = cfg->GetValue((const UTF8Char*)"GPIOPin");
		if (csptr && Text::StrToUInt16(csptr, &pinNum))
		{
			if (pinNum > 0)
			{
				NEW_CLASS(gpioCtrl, IO::GPIOControl());
				NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
				if (gpioCtrl->IsError() || pin->IsError())
				{
					DEL_CLASS(pin);
					console.WriteLine((const UTF8Char*)"Error in opening GPIO pin");
				}
				else
				{
					NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"evt"));
					threadRunning = false;
					threadToStop = false;
					pin->SetPinOutput(true);
					Sync::Thread::Create(ThreadFunc, 0);
					console.WriteLine((const UTF8Char*)"GPIO Sig Gen Running");
					progCtrl->WaitForExit(progCtrl);
					console.WriteLine((const UTF8Char*)"Exiting");
					threadToStop = true;
					evt->Set();
					while (threadRunning)
					{
						Sync::Thread::Sleep(1);
					}
					pin->SetPinOutput(false);
					DEL_CLASS(evt);
					DEL_CLASS(pin);
				}
				DEL_CLASS(gpioCtrl);
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Config GPIOPin not valid");
			}
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Config GPIOPin not found");
		}
		DEL_CLASS(cfg);
	}
	else
	{
		console.WriteLine((const UTF8Char*)"Config file not found");
	}
	return 0;
}
