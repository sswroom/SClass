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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::HiResClock clk;
	IO::ConsoleWriter console;
	Double t;
	OSInt i;
	while ((t = clk.GetTimeDiff()) == 0);
	Text::StringBuilderUTF8 sb;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("__cplusplus = "));
#if defined(__cplusplus)
	sb.AppendI32((Int32)__cplusplus);
#else
	sb.AppendC(UTF8STRC("Not defined"));
#endif
	console.WriteLine(sb.ToCString());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Finest time in HiResClock = "));
	sb.AppendDouble(t);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		t = clk.GetTimeDiff();
	}
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time per request in HiResClock = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());

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
	sb.AppendC(UTF8STRC("Time per request in gettimeofday = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());

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
	sb.AppendC(UTF8STRC("Time per request in timespec_get = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());
#endif

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock();
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time per request in clock = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock_gettime(CLOCK_REALTIME, &ts);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time per request in clock_gettime(CLOCK_REALTIME) = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());

	clk.Start();
	i = 10000;
	while (i-- > 0)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Time per request in clock_gettime(CLOCK_MONOTONIC) = "));
	sb.AppendDouble(t / 10000.0);
	console.WriteLine(sb.ToCString());
#endif
	return 0;
}
