#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[256];
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
		Text::StrInt64(sbuff, i);
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
		Text::StrInt64(sbuff, i);
	}
	Double t2b = clk->GetTimeDiff();

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"t1 = ");
	Text::SBAppendF64(&sb, t1);
	sb.Append((const UTF8Char*)", t2 = ");
	Text::SBAppendF64(&sb, t2);
	sb.Append((const UTF8Char*)"\r\nt1b = ");
	Text::SBAppendF64(&sb, t1b);
	sb.Append((const UTF8Char*)", t2b = ");
	Text::SBAppendF64(&sb, t2b);
	console->WriteLine(sb.ToString());
	DEL_CLASS(console);
	return 0;
}
