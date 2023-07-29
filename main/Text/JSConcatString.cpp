#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListString.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	OSInt i;
	OSInt j;
	Text::String *strArr[10000];
	NotNullPtr<Text::String> str;
	Data::ArrayListString *a;
	Double t1;
	Manage::HiResClock *clk;
	IO::ConsoleWriter console;

	NEW_CLASS(clk, Manage::HiResClock());

	i = 10000;
	while (i-- > 0)
	{
		strArr[i] = Text::String::New(UTF8STRC("1234567890")).Ptr();
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
		str->Release();
	}
	t1 = clk->GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Time = "));
	sb.AppendDouble(t1);
	console.WriteLineC(sb.ToString(), sb.GetLength());
	
	i = 10000;
	while (i-- > 0)
	{
		strArr[i]->Release();
	}
	DEL_CLASS(clk);
	return 0;
}
