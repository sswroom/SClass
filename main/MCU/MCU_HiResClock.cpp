#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[32];
	Sync::Thread::Sleep(10000);
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	while (true)
	{
		clk.Start();
		Sync::Thread::Sleep(1000);
		Text::StrInt64(sbuff, clk.GetTimeDiffus());
		console.Write((const UTF8Char*)"Time used(us): ");
		console.WriteLine(sbuff);
	}
	return 0;
}

