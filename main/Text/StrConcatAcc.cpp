#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Char sbuff[32];
	const Char *srcStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	IO::ConsoleWriter *console;
	NEW_CLASS(console, IO::ConsoleWriter());
	Text::StrConcat(sbuff, srcStr);
	if (Text::StrEquals(sbuff, srcStr))
	{
		console->WriteLineC(UTF8STRC("Success"));
	}
	DEL_CLASS(console);
	return 0;
}
