#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	Double t1;
	Double t2;
	UOSInt cnt = 10000;
	UOSInt i;
	UOSInt res = 0;
	UOSInt res2;
	Text::StringBuilderUTF8 sb;
	
	clk.Start();
	i = cnt;
	while (i-- > 0)
	{
		res = Text::StrCharCnt(srcStr);
	}
	t1 = clk.GetTimeDiff();
	clk.Start();
	i = cnt;
	while (i-- > 0)
	{
		res2 = strlen((const Char*)srcStr);
	}
	t2 = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Leng = "));
	sb.AppendUOSInt(res);
	sb.AppendC(UTF8STRC(", strlen = "));
	sb.AppendUOSInt(res2);
	sb.AppendC(UTF8STRC(", t1 = "));
	sb.AppendDouble(t1);
	sb.AppendC(UTF8STRC(", t1 = "));
	sb.AppendDouble(t2);
	console.WriteLine(sb.ToCString());
	return 0;
}
