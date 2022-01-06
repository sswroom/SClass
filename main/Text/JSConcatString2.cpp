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
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Manage::HiResClock *clk;
	UOSInt i;
	UOSInt j;
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
			s = Text::String::New(sbuff, (UOSInt)(Text::StrInt32(sbuff, (Int32)j) - sbuff));
			strs->Add(s);
		}
		Text::String *str = strs->JoinString();
		str->Release();
		j = strs->GetCount();
		while (j-- > 0)
		{
			strs->GetItem(j)->Release();
		}
		DEL_CLASS(strs);
	}
	Double t1 = clk->GetTimeDiff();
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, t1), UTF8STRC(" s"));
	console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	DEL_CLASS(console);

	DEL_CLASS(clk);
	return 0;
}
