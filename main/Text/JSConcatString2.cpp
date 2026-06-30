#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Manage::HiResClock> clk;
	UIntOS i;
	UIntOS j;
	NN<Data::ArrayListStringNN> strs;
	NN<Text::String> s;
	
	NEW_CLASSNN(clk, Manage::HiResClock());
	clk->Start();
	i = 1;
	while (i-- > 0)
	{
		NEW_CLASSNN(strs, Data::ArrayListStringNN());
		j = 1000;
		while (j-- > 0)
		{
			s = Text::String::New(sbuff, (UIntOS)(Text::StrInt32(sbuff, (Int32)j) - sbuff));
			strs->Add(s);
		}
		NN<Text::String> str = strs->JoinString();
		str->Release();
		j = strs->GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(strs->GetItem(j));
		}
		strs.Delete();
	}
	Double t1 = clk->GetTimeDiff();
	NN<IO::ConsoleWriter> console;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, t1), UTF8STRC(" s"));
	console->WriteLine(CSTRP(sbuff, sptr));
	console.Delete();

	clk.Delete();
	return 0;
}
