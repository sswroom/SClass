#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/GPIOControl.h"
#include "Sync/Thread.h"

#define PIN1 5
#define PIN2 8

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt i = 3;
	IO::GPIOControl gpio;	
	gpio.SetPinOutput(PIN1, true);
	gpio.SetPinOutput(PIN2, true);
	while (i-- > 0)
	{
		gpio.SetPinState(PIN1, true);
		gpio.SetPinState(PIN2, false);
		Sync::Thread::Sleep(1000);
		gpio.SetPinState(PIN1, false);
		gpio.SetPinState(PIN2, true);
		Sync::Thread::Sleep(1000);
	}
	return 0;
}
