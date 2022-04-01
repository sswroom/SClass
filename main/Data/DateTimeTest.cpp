#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"

#if !defined(_WIN32) && !defined(_WIN32_WCE) && !defined(CPU_AVR)
#include <sys/time.h>
#endif

#if !defined(CPU_AVR)
#include <time.h>
#include <stdio.h>
#endif

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

#if !defined(_WIN32) && !defined(_WIN32_WCE) && !defined(CPU_AVR)
	struct timeval tv;
	struct timezone tz;
	if (gettimeofday(&tv, &tz) == 0)
	{
		dt.SetTicks(1000 * (Int64)tv.tv_sec + tv.tv_usec / 1000);
		dt.SetTimeZoneQHR((Int8)(-tz.tz_minuteswest / 15));
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("gettimeofday: ")), "yyyy-MM-dd HH:mm:ss.fffzzzz");
		console.WriteLineCStr(CSTRP(sbuff, sptr));
	}
#endif

	time_t now = time(0);
	tm *t = localtime(&now);
	Int32 newTZ = (Int32)(t->tm_gmtoff / 900);
	dt.SetTimeZoneQHR((Int8)newTZ);

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	dt.SetUnixTimestamp(ts.tv_sec);
	dt.SetMS((UInt16)(ts.tv_nsec / 1000000));
	sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("clock_gettime: ")), "yyyy-MM-dd HH:mm:ss.fffzzzz");
	console.WriteLineCStr(CSTRP(sbuff, sptr));

	return 0;
}

