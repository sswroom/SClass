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

UInt32 __stdcall ThreadFunc(AnyType userObj)
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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	NN<Text::String> s;
	UInt16 pinNum;
	if (cfg)
	{
		pinNum = 0;
		if (cfg->GetValue(CSTR("GPIOPin")).SetTo(s) && s->ToUInt16(pinNum))
		{
			if (pinNum > 0)
			{
				IO::GPIOControl gpioCtrl;
				NEW_CLASS(pin, IO::GPIOPin(gpioCtrl, pinNum));
				if (gpioCtrl.IsError() || pin->IsError())
				{
					DEL_CLASS(pin);
					console.WriteLine(CSTR("Error in opening GPIO pin"));
				}
				else
				{
					NEW_CLASS(evt, Sync::Event(true));
					threadRunning = false;
					threadToStop = false;
					pin->SetPinOutput(true);
					Sync::ThreadUtil::Create(ThreadFunc, 0);
					console.WriteLine(CSTR("GPIO Sig Gen Running"));
					progCtrl->WaitForExit(progCtrl);
					console.WriteLine(CSTR("Exiting"));
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
			}
			else
			{
				console.WriteLine(CSTR("Config GPIOPin not valid"));
			}
		}
		else
		{
			console.WriteLine(CSTR("Config GPIOPin not found"));
		}
		DEL_CLASS(cfg);
	}
	else
	{
		console.WriteLine(CSTR("Config file not found"));
	}
	return 0;
}
