#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GPIOControl.h"
#include "IO/Device/HCSR04.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#define TRIGPIN 1
#define ECHOPIN 0

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::GPIOControl gpio;
	IO::Device::HCSR04 distSensor(&gpio, TRIGPIN, ECHOPIN);
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Int32 t0;
	Int32 t1;
	Int32 t2;
	Int32 tv1;
	Int32 tv2;
	Sync::SimpleThread::Sleep(10000);
	while (true)
	{
		t0 = 0;
		t1 = 0;
		t2 = 0;
		distSensor.ReadTime(&t0);
		Sync::SimpleThread::Sleep(100);
		distSensor.ReadTime(&t1);
		Sync::SimpleThread::Sleep(100);
		distSensor.ReadTime(&t2);

		if (t0 != 0 && t1 != 0 && t2 != 0)
		{
			tv1 = t0 - t1;
			tv2 = t0 - t2;
			if (tv1 >= -10 && tv1 <= 10 && tv2 >= -10 && tv2 <= 10)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("distance(mm) = "));
				sptr = Text::StrInt32(sptr, distSensor.ConvDistanceMM_RoomTemp(t0));

				console.WriteLine(CSTRP(sbuff, sptr));
			}
		}
		Sync::SimpleThread::Sleep(1000);
	}
	return 0;
}

