#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	WChar wbuff[32];
	OSInt i = 10000000;
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	Manage::HiResClock clk;
	Double t;
	NEW_CLASS(console, IO::ConsoleWriter());

	clk.Start();
	while (i-- > 0)
	{
		Text::StrConcatS(wbuff, srcStr, 26);
	}
	t = clk.GetTimeDiff();

	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t = "));
	sb.AppendDouble(t);
	console->WriteLine(sb.ToCString());
	DEL_CLASS(console);
	return 0;
}
