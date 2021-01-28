#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	Manage::HiResClock clk;
	Double t;
	OSInt i = 10000000;
	OSInt res;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	
	clk.Start();
	while (i-- > 0)
	{
		res = Text::StrCharCnt(srcStr);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Leng = ");
	sb.AppendOSInt(res);
	sb.Append((const UTF8Char*)", t = ");
	Text::SBAppendF64(&sb, t);
	console->WriteLine(sb.ToString());
	DEL_CLASS(console);
	return 0;
}
