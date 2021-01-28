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
	const WChar *srcStr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcat(sbuff, srcStr);
	if (Text::StrEquals(sbuff, srcStr))
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
