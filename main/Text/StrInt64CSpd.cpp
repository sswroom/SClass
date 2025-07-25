#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	WChar wbuff[256];
	Char sbuff2[256];
	Manage::HiResClock *clk;

	NEW_CLASS(clk, Manage::HiResClock());

	Int64 i;
	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		Text::StrInt64(sbuff2, i);
	}
	Double t1 = clk->GetTimeDiff();

	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		Text::StrInt64(wbuff, i);
	}
	Double t1b = clk->GetTimeDiff();

	clk->Start();
	i = 100010000000LL;
	while (i-- > 100000000000LL)
	{
		Text::StrInt64(sbuff2, i);
	}
	Double t2 = clk->GetTimeDiff();

	clk->Start();
	i = 100010000000LL;
	while (i-- > 100000000000LL)
	{
		Text::StrInt64(wbuff, i);
	}
	Double t2b = clk->GetTimeDiff();

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t1 = "));
	sb.AppendDouble(t1);
	sb.AppendC(UTF8STRC(", t2 = "));
	sb.AppendDouble(t2);
	sb.AppendC(UTF8STRC("\r\nt1b = "));
	sb.AppendDouble(t1b);
	sb.AppendC(UTF8STRC(", t2b = "));
	sb.AppendDouble(t2b);
	console->WriteLine(sb.ToCString());
	DEL_CLASS(console);
	return 0;
}
