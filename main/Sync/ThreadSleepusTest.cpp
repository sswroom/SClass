#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Manage::HiResClock *clk;
	UInt32 i;
	IntOS j;
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
			Sync::SimpleThread::Sleepus(i);
		}
		t = clk->GetTimeDiff();
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Sleep 10 x "));
		sb.AppendU32(i);
		sb.AppendC(UTF8STRC(", t = "));
		sb.AppendDouble(t);
		console.WriteLine(sb.ToCString());
		i = i << 1;
	}

	DEL_CLASS(clk);
	return 0;
}
