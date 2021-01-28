#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListString.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff2[256];
	Manage::HiResClock *clk;
	OSInt i;
	OSInt j;
	Data::ArrayListString *strs;
	Text::String *s;
	
	NEW_CLASS(clk, Manage::HiResClock());
	clk->Start();
	i = 1;
	while (i-- > 0)
	{
		NEW_CLASS(strs, Data::ArrayListString());
		j = 1000;
		while (j-- > 0)
		{
			NEW_CLASS(s, Text::String(sbuff2, Text::StrInt32(sbuff2, (Int32)j)));
			strs->Add(s);
		}
		Text::String *str = strs->JoinString();
		DEL_CLASS(str);
		j = strs->GetCount();
		while (j-- > 0)
		{
			DEL_CLASS(strs->GetItem(j));
		}
		DEL_CLASS(strs);
	}
	Double t1 = clk->GetTimeDiff();
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcat(Text::StrDouble(sbuff2, t1), (const UTF8Char*)" s");
	console->WriteLine(sbuff2);
	DEL_CLASS(console);

	DEL_CLASS(clk);
	return 0;
}
