#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#if defined(_WIN32_WCE)
#include "Manage/HiResClock.h"
#endif
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <windows.h>

void Sync::Thread::Sleep(UOSInt ms)
{
	::Sleep((UInt32)ms);
}

extern "C"
{
	void Thread_NanoSleep(Int64 clk);
};
static UInt64 Thread_Freq = 0;

void Sync::Thread::Sleepus(UOSInt us)
{
#if defined(_WIN32_WCE)
	Manage::HiResClock clk;
	while (clk.GetTimeDiffus() < us)
	{
	}
#else
	if (Thread_Freq == 0)
	{
		LARGE_INTEGER liFreq;
		QueryPerformanceFrequency(&liFreq);
		Thread_Freq = (UInt64)liFreq.QuadPart;
	}
	Int64 clkCount = (Int64)(Thread_Freq * ((us << 1) + 1) / 2000LL);
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
		Thread_NanoSleep(clkCount);
	}
#endif
}

UInt32 Sync::Thread::Create(Sync::ThreadProc tProc, void *userObj)
{
	DWORD threadId = 0;
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)tProc, userObj, 0, &threadId));
	return threadId;
}

UInt32 Sync::Thread::Create(Sync::ThreadProc tProc, void *userObj, UInt32 threadSize)
{
	DWORD threadId = 0;
	CloseHandle(CreateThread(0, threadSize, (LPTHREAD_START_ROUTINE)tProc, userObj, STACK_SIZE_PARAM_IS_A_RESERVATION, &threadId));
	return threadId;
}

UInt32 Sync::Thread::GetThreadId()
{
	return GetCurrentThreadId();
}

UOSInt Sync::Thread::GetThreadCnt()
{
	SYSTEM_INFO sysInfo;

	GetSystemInfo(&sysInfo);
	if (sysInfo.dwNumberOfProcessors <= 0)
		return 1;
	else
		return sysInfo.dwNumberOfProcessors;
}

void Sync::Thread::SetPriority(ThreadPriority priority)
{
	Int32 threadPriority;
	switch (priority)
	{
	case TP_IDLE:
		threadPriority = THREAD_PRIORITY_IDLE;
		break;
	case TP_LOWEST:
		threadPriority = THREAD_PRIORITY_LOWEST;
		break;
	case TP_LOW:
		threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case TP_NORMAL:
		threadPriority = THREAD_PRIORITY_NORMAL;
		break;
	case TP_HIGH:
		threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case TP_HIGHEST:
		threadPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case TP_REALTIME:
		threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default:
		return;
	}
	SetThreadPriority(GetCurrentThread(), threadPriority);
}
