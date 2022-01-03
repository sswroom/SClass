#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Manage::HiResClock *clk;
	UInt32 i;
	OSInt j;
	Double t;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(clk, Manage::HiResClock());
	i = 1;
	while (i <= 2048)
	{
		j = 10;
		clk->Start();
		while (j-- > 0)
		{
			Sync::Thread::Sleepus(i);
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Sleep 10 x "));
		sb.AppendU32(i);
		sb.AppendC(UTF8STRC(", t = "));
		Text::SBAppendF64(&sb, t);
		console.WriteLine(sb.ToString());
		i = i << 1;
	}

	DEL_CLASS(clk);
	return 0;
}
