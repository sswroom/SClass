#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("Curr Time (UTC):")), "yyyy-MM-dd HH:mm:ss.fffzzzz");
	console.WriteLineCStr(CSTRP(sbuff, sptr));
	dt.ToLocalTime();
	sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("Curr Time (ToLocal):")), "yyyy-MM-dd HH:mm:ss.fffzzzz");
	console.WriteLineCStr(CSTRP(sbuff, sptr));
	dt.SetCurrTime();
	sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("Curr Time (Local):")), "yyyy-MM-dd HH:mm:ss.fffzzzz");
	console.WriteLineCStr(CSTRP(sbuff, sptr));
	return 0;
}

