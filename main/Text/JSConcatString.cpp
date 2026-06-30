#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListString.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IntOS i;
	IntOS j;
	Text::String *strArr[10000];
	NN<Text::String> str;
	NN<Data::ArrayListString> a;
	Double t1;
	NN<Manage::HiResClock> clk;
	IO::ConsoleWriter console;

	NEW_CLASSNN(clk, Manage::HiResClock());

	i = 10000;
	while (i-- > 0)
	{
		strArr[i] = Text::String::New(UTF8STRC("1234567890")).Ptr();
	}

	clk->Start();
	j = 9000;
	while (j-- > 0)
	{
		NEW_CLASSNN(a, Data::ArrayListString(1000));
		i = 1000;
		while (i-- > 0)
		{
			a->Add(strArr[i]);
		}
		str = a->JoinString();
		a.Delete();
		str->Release();
	}
	t1 = clk->GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Time = "));
	sb.AppendDouble(t1);
	console.WriteLine(sb.ToCString());
	
	i = 10000;
	while (i-- > 0)
	{
		strArr[i]->Release();
	}
	clk.Delete();
	return 0;
}
