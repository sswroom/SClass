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
	Double spd1;
	Double spd2;
	UOSInt i;
	OSInt val = 0;
	OSInt val2 = 0;
	Manage::HiResClock *clk;
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;

	NEW_CLASS(clk, Manage::HiResClock());
	i = 10000000;
	clk->Start();
	while (i-- > 0)
	{
		val = Text::StrCompareICase(L"aBcDe1FgHiJk", L"AbCdE1fGhIjK");
	}
	spd1 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Compare value1 = "));
	sb.AppendOSInt(val);
	sb.AppendC(UTF8STRC(", t = "));
	Text::SBAppendF64(&sb, spd1);
	console.WriteLineC(sb.ToString(), sb.GetLength());

	i = 10000000;
	clk->Start();
	while (i-- > 0)
	{
		val2 = Text::StrCompare(L"aBcDe1FgHiJk", L"AbCdE1fGhIjK");
	}
	spd2 = clk->GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Compare value2 = "));
	sb.AppendOSInt(val2);
	sb.AppendC(UTF8STRC(", t = "));
	Text::SBAppendF64(&sb, spd2);
	console.WriteLineC(sb.ToString(), sb.GetLength());

	DEL_CLASS(clk);

	return 0;
}
