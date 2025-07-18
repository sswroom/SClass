#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include <wchar.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	WChar wbuff[256];
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Manage::HiResClock *clk;

	NEW_CLASS(clk, Manage::HiResClock());
	Int32 i;

	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		Text::StrInt32(wbuff, i);
	}
	Double t1 = clk->GetTimeDiff();

	clk->Start();
	i = 10000000;
	while (i-- > 0)
	{
		swprintf(wbuff, 11, L"%d", i);
	}
	Double t2 = clk->GetTimeDiff();

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(sbuff, UTF8STRC("t1 = ")), t1), UTF8STRC(" s"));
	console->WriteLine(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(sbuff, UTF8STRC("t2 = ")), t2), UTF8STRC(" s"));
	console->WriteLine(CSTRP(sbuff, sptr));
	DEL_CLASS(console);
	return 0;
}
