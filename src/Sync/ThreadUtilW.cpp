#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Library.h"
#include "IO/StreamReader.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

void Sync::ThreadUtil::SleepDur(Data::Duration dur)
{
	Int64 ms = dur.GetTotalMS();
	UInt32 us = (dur.GetNS() % 1000000) / 1000;
	if (ms > 0)
		Sync::SimpleThread::Sleep((UOSInt)ms);
	if (us > 0)
		Sync::SimpleThread::Sleepus(us);
}

void Sync::ThreadUtil::Create(Sync::ThreadProc tProc, AnyType userObj)
{
	CloseHandle(CreateWithHandle(tProc, userObj));
}

void Sync::ThreadUtil::Create(Sync::ThreadProc tProc, AnyType userObj, UInt32 threadSize)
{
	CloseHandle(CreateWithHandle(tProc, userObj, threadSize));
}

Sync::ThreadHandle *Sync::ThreadUtil::CreateWithHandle(Sync::ThreadProc tProc, AnyType userObj)
{
	DWORD threadId = 0;
	return (ThreadHandle*)CreateThread(0, 0, (LPTHREAD_START_ROUTINE)tProc, userObj.p, 0, &threadId);
}

Sync::ThreadHandle *Sync::ThreadUtil::CreateWithHandle(Sync::ThreadProc tProc, AnyType userObj, UInt32 threadSize)
{
	DWORD threadId = 0;
	return (ThreadHandle*)CreateThread(0, threadSize, (LPTHREAD_START_ROUTINE)tProc, userObj.p, STACK_SIZE_PARAM_IS_A_RESERVATION, &threadId);
}

void Sync::ThreadUtil::CloseHandle(ThreadHandle *handle)
{
	::CloseHandle((HANDLE)handle);
}

UInt32 Sync::ThreadUtil::GetThreadId(ThreadHandle *handle)
{
	return ::GetThreadId((HANDLE)handle);
}

UInt32 Sync::ThreadUtil::GetThreadId()
{
	return GetCurrentThreadId();
}

UOSInt Sync::ThreadUtil::GetThreadCnt()
{
	SYSTEM_INFO sysInfo;

	GetSystemInfo(&sysInfo);
	if (sysInfo.dwNumberOfProcessors <= 0)
		return 1;
	else
		return sysInfo.dwNumberOfProcessors;
}

void Sync::ThreadUtil::SetPriority(ThreadPriority priority)
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

typedef HRESULT (WINAPI *SetThreadDescriptionFunc)(HANDLE hThread, PCWSTR lpThreadDescription);

Bool Sync::ThreadUtil::SetName(Text::CStringNN name)
{
	IO::Library lib((const UTF8Char*)"Kernel32.dll");
	SetThreadDescriptionFunc SetThreadDescription = (SetThreadDescriptionFunc)lib.GetFunc("SetThreadDescription");
	if (SetThreadDescription == 0)
		return false;
	const WChar *wName = Text::StrToWCharNew(name.v);
	HRESULT hr = SetThreadDescription(GetCurrentThread(), wName);
	Text::StrDelNew(wName);
	return hr == 0;
}
