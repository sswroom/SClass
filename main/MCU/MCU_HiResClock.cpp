#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Sync::ThreadUtil::SleepDur(10000);
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	while (true)
	{
		clk.Start();
		Sync::ThreadUtil::SleepDur(1000);
		sptr = Text::StrInt64(sbuff, clk.GetTimeDiffus());
		console.Write(CSTR("Time used(us): "));
		console.WriteLine(CSTRP(sbuff, sptr));
	}
	return 0;
}

