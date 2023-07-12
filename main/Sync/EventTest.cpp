#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Manage::HiResClock *clk; Sync::Event *evt; Double t1; Double t2;

UInt32 __stdcall TestThread(void *userObj)
{
	evt->Wait();
	t1 = clk->GetTimeDiff();
	return 0;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NEW_CLASS(clk, Manage::HiResClock());
	NEW_CLASS(evt, Sync::Event(false));
	Sync::ThreadUtil::Create(TestThread, 0);
	Sync::SimpleThread::Sleep(10);
	clk->Start();
	evt->Set();
	t2 = clk->GetTimeDiff();
	Sync::SimpleThread::Sleep(10);

	DEL_CLASS(evt);
	DEL_CLASS(clk);

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t1 = "));
	Text::SBAppendF64(&sb, t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	Text::SBAppendF64(&sb, t2);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(console);
	return 0;
}
