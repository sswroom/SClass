#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

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

void CurrTimeTest()
{
	UTF8Char sbuff[64];
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Int64 initTicks = dt.ToTicks();
	UOSInt i = 10;
	while (i-- > 0)
	{
		Sync::Thread::Sleep(10);
		dt.SetCurrTimeUTC();
		Text::StrInt64(sbuff, dt.ToTicks() - initTicks);
		console->WriteLine(sbuff);
	}
	dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
	console->WriteLine(sbuff);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	DateTest("2020-12-31T16:01:02.123+04:00");
	CurrTimeTest();
	DEL_CLASS(console);
	return 0;
}
