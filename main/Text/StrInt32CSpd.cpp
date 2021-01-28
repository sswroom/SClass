#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[256];
	UTF8Char sbuff2[256];
	Manage::HiResClock *clk;

	NEW_CLASS(clk, Manage::HiResClock());
	Int32 i;

	clk->Start();
	i = 100000000;
	while (i-- > 0)
	{
		Text::StrInt32(sbuff2, i);
	}
	Double t1 = clk->GetTimeDiff();

	clk->Start();
	i = 100000000;
	while (i-- > 0)
	{
		Text::StrInt32(sbuff, i);
	}
	Double t2 = clk->GetTimeDiff();

	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcat(Text::StrDouble(Text::StrConcat(sbuff2, (const UTF8Char*)"t1 = "), t1), (const UTF8Char*)" s");
	console->WriteLine(sbuff2);
	Text::StrConcat(Text::StrDouble(Text::StrConcat(sbuff2, (const UTF8Char*)"t2 = "), t2), (const UTF8Char*)" s");
	console->WriteLine(sbuff2);
	DEL_CLASS(console);
	return 0;
}
