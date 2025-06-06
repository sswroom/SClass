#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
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
		console.Write(CSTR("Time used(us): "));
		console.WriteLine(CSTRP(sbuff, sptr));
	}
	return 0;
}

