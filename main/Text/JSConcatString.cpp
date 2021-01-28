#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListString.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt i;
	OSInt j;
	Text::String *strArr[10000];
	Text::String *str;
	Data::ArrayListString *a;
	Double t1;
	Manage::HiResClock *clk;
	IO::ConsoleWriter console;

	NEW_CLASS(clk, Manage::HiResClock());

	i = 10000;
	while (i-- > 0)
	{
		NEW_CLASS(strArr[i], Text::String((const UTF8Char*)"1234567890"));
	}

	clk->Start();
	j = 9000;
	while (j-- > 0)
	{
		NEW_CLASS(a, Data::ArrayListString(1000));
		i = 1000;
		while (i-- > 0)
		{
			a->Add(strArr[i]);
		}
		str = a->JoinString();
		DEL_CLASS(a);
		DEL_CLASS(str);
	}
	t1 = clk->GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Time = ");
	Text::SBAppendF64(&sb, t1);
	console.WriteLine(sb.ToString());
	
	i = 10000;
	while (i-- > 0)
	{
		DEL_CLASS(strArr[i]);
	}
	DEL_CLASS(clk);
	return 0;
}
