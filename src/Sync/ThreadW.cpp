#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <windows.h>

void Sync::Thread::SleepDur(Data::Duration dur)
{
	Int64 ms = dur.GetTotalMS();
	UInt32 us = (dur.GetNS() % 1000000) / 1000;
	if (ms > 0)
		Sync::SimpleThread::Sleep((UOSInt)ms);
	if (us > 0)
		Sync::SimpleThread::Sleepus(us);
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

Bool Sync::Thread::EnableInterrupt()
{
	return false;
}

Bool Sync::Thread::Interrupt(UInt32 threadId)
{
	return false;
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
