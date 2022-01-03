#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Manage::HiResClock *clk; Sync::Event *evt; Double t1; Double t2;

UInt32 __stdcall TestThread(void *userObj)
{
	evt->Wait();
	t1 = clk->GetTimeDiff();
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(evt, Sync::Event(false, (const UTF8Char*)"Test"));
	Sync::Thread::Create(TestThread, 0);
	Sync::Thread::Sleep(10);
	clk->Start();
	evt->Set();
	t2 = clk->GetTimeDiff();
	Sync::Thread::Sleep(10);

	DEL_CLASS(evt);
	DEL_CLASS(clk);

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t1 = "));
	Text::SBAppendF64(&sb, t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	Text::SBAppendF64(&sb, t2);
	console->WriteLine(sb.ToString());
	DEL_CLASS(console);
	return 0;
}
