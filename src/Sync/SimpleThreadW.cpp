#include "Stdafx.h"
#if defined(_WIN32_WCE)
#include "Manage/HiResClock.h"
#endif
#include "Sync/SimpleThread.h"
#include <windows.h>

void Sync::SimpleThread::Sleep(UIntOS ms)
{
	::Sleep((UInt32)ms);
}

extern "C"
{
	void SimpleThread_NanoSleep(Int64 clk);
};
static UInt64 SimpleThread_Freq = 0;

void Sync::SimpleThread::Sleepus(UIntOS us)
{
#if defined(_WIN32_WCE)
	Manage::HiResClock clk;
	while (clk.GetTimeDiffus() < us)
	{
	}
#else
	if (SimpleThread_Freq == 0)
	{
		LARGE_INTEGER liFreq;
		QueryPerformanceFrequency(&liFreq);
		SimpleThread_Freq = (UInt64)liFreq.QuadPart;
	}
	Int64 clkCount = (Int64)(SimpleThread_Freq * ((us << 1) + 1) / 2000LL);
	if (us >= 1000)
	{
		LARGE_INTEGER liStart;
		LARGE_INTEGER liEnd;
		QueryPerformanceCounter(&liStart);
		::Sleep((UInt32)(us / 1000));
		QueryPerformanceCounter(&liEnd);
		clkCount -= (liEnd.QuadPart - liStart.QuadPart) * 1000;
	}
	if (clkCount > 0)
	{
		SimpleThread_NanoSleep(clkCount);
	}
#endif
}
