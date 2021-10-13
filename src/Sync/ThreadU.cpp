#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <wchar.h>

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

void Sync::Thread::Sleep(UOSInt ms)
{
	UOSInt s = ms / 1000;
	ms = ms % 1000;
	if (s)
		if (!sleep((UInt32)s))
			return;
	if (ms)
		if (!usleep((useconds_t)(ms * 1000)))
			return;
}

void Sync::Thread::Sleepus(UOSInt us)
{
	struct timeval tNow, tLong, tEnd;
	gettimeofday (&tNow, 0) ;
	tLong.tv_sec  = (time_t)(us / 1000000);
	tLong.tv_usec = (suseconds_t)(us % 1000000);
	timeradd (&tNow, &tLong, &tEnd) ;

	if (us >= 60)
	{
		struct timespec sleeper ;
		us = us - 59;
		sleeper.tv_sec  = (time_t)(us / 1000000);
		sleeper.tv_nsec = (Int32)((us % 1000000) * 1000L) ;
		nanosleep (&sleeper, NULL) ;

		gettimeofday (&tNow, 0) ;
	}
	while (timercmp (&tNow, &tEnd, <))
		gettimeofday (&tNow, 0) ;
}

UInt32 Sync::Thread::Create(Sync::ThreadProc tProc, void *userObj)
{
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	while (true)
	{
		int ret = pthread_create(&tid, &attr, (void*(*)(void*))tProc, userObj);
		if (ret == 0)
		{
			break;
		}
		else if (ret == 11)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			wprintf(L"Error in creating thread: %d\r\n", ret);
			break;
		}
	}
#if defined(__APPLE_)
	int id;
	pthread_getunique_np(&tid, &id);
	return id;
#else
	return (UInt32)(UOSInt)tid;
#endif
}

UInt32 Sync::Thread::Create(Sync::ThreadProc tProc, void *userObj, UInt32 threadSize)
{
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, threadSize);
	while (true)
	{
		int ret = pthread_create(&tid, &attr, (void*(*)(void*))tProc, userObj);
		if (ret == 0)
		{
			break;
		}
		else if (ret == 11)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			wprintf(L"Error in creating thread: %d\r\n", ret);
			break;
		}
	}
#if defined(__APPLE_)
	int id;
	pthread_getunique_np(&tid, &id);
	return id;
#else
	return (UInt32)(UOSInt)tid;
#endif
}

UInt32 Sync::Thread::GetThreadId()
{
#if defined(__linux__)
	return (UInt32)gettid();
#else
	return (UInt32)(UOSInt)pthread_self();
#endif
}

UOSInt Sync::Thread::GetThreadCnt()
{
#if defined(__FreeBSD__)
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	int ncpu = 1;
	size_t size = sizeof(ncpu);
	if (sysctl(mib, 2, &ncpu, &size, 0, 0) == 0)
	{
		return ncpu;
	}
	return 1;
#else
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UOSInt procCnt;
	OSInt procSys;
	UTF8Char u8buff[128];
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/cpuinfo", IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	procCnt = 0;
	while (reader->ReadLine(u8buff, 127))
	{
		if (Text::StrStartsWith(u8buff, (const UTF8Char*)"processor\t"))
		{
			procCnt++;
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	procSys = sysconf(_SC_NPROCESSORS_CONF);
	if (procSys > 0 && (UOSInt)procSys >= procCnt)
		return (UOSInt)procSys;
	if (procCnt == 0)
		return 1;
	return procCnt;
#endif
}

void Sync::Thread::SetPriority(ThreadPriority priority)
{
/*	Int32 threadPriority;
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
	SetThreadPriority(GetCurrentThread(), threadPriority);*/
}
