#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Char sbuff[32];
	OSInt i = 10000000;
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	Manage::HiResClock clk;
	Double t;
	NEW_CLASS(console, IO::ConsoleWriter());

	clk.Start();
	while (i-- > 0)
	{
		Text::StrConcatC(sbuff, srcStr, 26);
	}
	t = clk.GetTimeDiff();

	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("t = "));
	sb.AppendDouble(t);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(console);
	return 0;
}
