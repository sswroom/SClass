#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include <wchar.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	WChar wbuff[256];
	Manage::HiResClock *clk;

	NEW_CLASS(clk, Manage::HiResClock());

	Int64 i;
	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		Text::StrInt64(wbuff, i);
	}
	Double t1 = clk->GetTimeDiff();

	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		swprintf(wbuff, 11, L"%ld", i);
	}
	Double t1b = clk->GetTimeDiff();

	clk->Start();
	i = 100010000000LL;
	while (i-- > 100000000000LL)
	{
		Text::StrInt64(wbuff, i);
	}
	Double t2 = clk->GetTimeDiff();

	clk->Start();
	i = 100010000000LL;
	while (i-- > 100000000000LL)
	{
		swprintf(wbuff, 21, L"%ld", i);
	}
	Double t2b = clk->GetTimeDiff();

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t1 = "));
	Text::SBAppendF64(&sb, t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	Text::SBAppendF64(&sb, t2);
	sb.AppendC(UTF8STRC("\r\nt1b = "));
	Text::SBAppendF64(&sb, t1b);
	sb.AppendC(UTF8STRC(", t2b = "));
	Text::SBAppendF64(&sb, t2b);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(console);
	return 0;
}
