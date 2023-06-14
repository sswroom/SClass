#include "Stdafx.h"
#include "MyMemory.h"
#include <windows.h>
#include <intrin.h>

#if defined(CPU_X86_32) || defined(CPU_X86_64)
extern "C" void SimpleThread_NanoSleep(Int64 clk)
{
	UInt64 endTime = __rdtsc() + clk;
	while (true)
	{
		YieldProcessor();
		YieldProcessor();
		YieldProcessor();
		YieldProcessor();
		if (__rdtsc() >= endTime)
			break;
	}
}
#else
extern "C" void SimpleThread_NanoSleep(Int64 clk)
{
	UInt64 endTime;
	LARGE_INTEGER liCurr;
	QueryPerformanceCounter(&liCurr);
	endTime = liCurr.QuadPart + clk;
	while (true)
	{
		YieldProcessor();
		YieldProcessor();
		YieldProcessor();
		YieldProcessor();
		QueryPerformanceCounter(&liCurr);
		if ((UInt64)liCurr.QuadPart >= endTime)
			break;
	}
}
#endif
