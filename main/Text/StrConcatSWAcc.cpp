#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilder.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WChar sbuff[32];
	WChar sbuff2[32];
	WChar sbuff3[32];
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcatS(sbuff, srcStr, 26);
	Text::StrConcatS(sbuff2, srcStr, 25);
	Text::StrConcatS(sbuff3, srcStr, 27);
	if (Text::StrEquals(sbuff, srcStr) && !Text::StrEquals(sbuff2, srcStr) && Text::StrEquals(sbuff3, srcStr))
	{
		console->WriteLine((const UTF8Char*)"Success");
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Fail");
	}
	DEL_CLASS(console);
	return 0;
}
