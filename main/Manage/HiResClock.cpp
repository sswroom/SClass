#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#if defined(__linux__)
#include <time.h>
#include <sys/time.h>
#endif

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::HiResClock clk;
	IO::ConsoleWriter console;
	Double t;
	OSInt i;
	while ((t = clk.GetTimeDiff()) == 0);
	Text::StringBuilderUTF8 sb;
	sb.ClearStr();
	sb.Append((const UTF8Char*)"__cplusplus = ");
#if defined(__cplusplus)
	sb.AppendI32((Int32)__cplusplus);
#else
	sb.Append((const UTF8Char*)"Not defined");
#endif
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Finest time in HiResClock = ");
	Text::SBAppendF64(&sb, t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		t = clk.GetTimeDiff();
	}
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in HiResClock = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());

#if defined(__linux__)
	struct timeval tval;
	struct timezone tz;
	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		gettimeofday(&tval, &tz);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in gettimeofday = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());

	struct timespec ts;
#if __cplusplus >= 201112L && !defined(__FreeBSD__) && defined(__USE_ISOC11)
	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		timespec_get(&ts, TIME_UTC);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in timespec_get = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());
#endif

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock();
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in clock = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock_gettime(CLOCK_REALTIME, &ts);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in clock_gettime(CLOCK_REALTIME) = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Time per request in clock_gettime(CLOCK_MONOTONIC) = ");
	Text::SBAppendF64(&sb, t / 10000.0);
	console.WriteLine(sb.ToString());
#endif
	return 0;
}
