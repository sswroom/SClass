#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"

IO::ConsoleWriter *console;

void DateTest(const Char *s)
{
	Data::DateTime dt;
	UTF8Char sbuff[64];
	console->WriteLine((const UTF8Char*)s);
	dt.SetValue(s);
	dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
	console->WriteLine(sbuff);
	console->WriteLine();
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	DateTest("2020-12-31T16:01:02.123+04:00");
	DEL_CLASS(console);
	return 0;
}

