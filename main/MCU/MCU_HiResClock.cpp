#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Sync::SimpleThread::Sleep(10000);
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	while (true)
	{
		clk.Start();
		Sync::SimpleThread::Sleep(1000);
		sptr = Text::StrInt64(sbuff, clk.GetTimeDiffus());
		console.WriteStrC(UTF8STRC("Time used(us): "));
		console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	return 0;
}

