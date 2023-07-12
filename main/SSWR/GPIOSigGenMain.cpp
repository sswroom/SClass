#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOPin.h"
#include "IO/IniFile.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::GPIOControl *gpioCtrl;
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	Text::String *s;
	UInt16 pinNum;
	if (cfg)
	{
		pinNum = 0;
		s = cfg->GetValue(CSTR("GPIOPin"));
		if (s && s->ToUInt16(&pinNum))
		{
			if (pinNum > 0)
			{
				NEW_CLASS(gpioCtrl, IO::GPIOControl());
				NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
				if (gpioCtrl->IsError() || pin->IsError())
				{
					DEL_CLASS(pin);
					console.WriteLineC(UTF8STRC("Error in opening GPIO pin"));
				}
				else
				{
					NEW_CLASS(evt, Sync::Event(true));
					threadRunning = false;
					threadToStop = false;
					pin->SetPinOutput(true);
					Sync::ThreadUtil::Create(ThreadFunc, 0);
					console.WriteLineC(UTF8STRC("GPIO Sig Gen Running"));
					progCtrl->WaitForExit(progCtrl);
					console.WriteLineC(UTF8STRC("Exiting"));
					threadToStop = true;
					evt->Set();
					while (threadRunning)
					{
						Sync::SimpleThread::Sleep(1);
					}
					pin->SetPinOutput(false);
					DEL_CLASS(evt);
					DEL_CLASS(pin);
				}
				DEL_CLASS(gpioCtrl);
			}
			else
			{
				console.WriteLineC(UTF8STRC("Config GPIOPin not valid"));
			}
		}
		else
		{
			console.WriteLineC(UTF8STRC("Config GPIOPin not found"));
		}
		DEL_CLASS(cfg);
	}
	else
	{
		console.WriteLineC(UTF8STRC("Config file not found"));
	}
	return 0;
}
