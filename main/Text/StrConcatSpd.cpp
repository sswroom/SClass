#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	OSInt i = 100000000;
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	Manage::HiResClock clk;
	Double t;
	NEW_CLASS(console, IO::ConsoleWriter());

	clk.Start();
	while (i-- > 0)
	{
		Text::StrConcat(sbuff, srcStr);
	}
	t = clk.GetTimeDiff();

	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"t = ");
	Text::SBAppendF64(&sb, t);
	console->WriteLine(sb.ToString());
	DEL_CLASS(console);
	return 0;
}
