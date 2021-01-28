#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	Char sbuff2[32];
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcatC(sbuff, srcStr, 26);
	Text::StrConcatC(sbuff2, srcStr, 25);
	if (Text::StrEquals(sbuff, srcStr) && !Text::StrEquals(sbuff2, srcStr))
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
