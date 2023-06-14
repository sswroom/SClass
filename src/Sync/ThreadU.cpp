#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

void Sync::Thread::SleepDur(Data::Duration dur)
{
	struct timeval tNow, tLong, tEnd;
	gettimeofday (&tNow, 0) ;
	tLong.tv_sec  = (time_t)dur.GetSeconds();
	tLong.tv_usec = (suseconds_t)(dur.GetNS() / 1000);
	timeradd (&tNow, &tLong, &tEnd) ;

	struct timespec sleeper ;
	sleeper.tv_sec  = (time_t)dur.GetSeconds();
	sleeper.tv_nsec = (Int32)dur.GetNS();
	nanosleep (&sleeper, NULL) ;
	gettimeofday (&tNow, 0) ;
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
			Sync::SimpleThread::Sleep(100);
		}
		else
		{
			printf("Error in creating thread: %d\r\n", ret);
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
	UInt32 thisThreadSize = threadSize;
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
			Sync::SimpleThread::Sleep(100);
		}
		else if (ret == 22)
		{
			if (thisThreadSize >= 1048576)
			{
				printf("Error in creating thread: %d with stack size %d\r\n", ret, threadSize);
				break;
			}
			thisThreadSize *= 2;
			pthread_attr_setstacksize(&attr, thisThreadSize);
		}
		else
		{
			printf("Error in creating thread: %d with stack size %d\r\n", ret, threadSize);
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
	cpu_set_t set;
	if (sched_getaffinity(getpid(), sizeof (set), &set) == 0)
    {
		UInt32 count;
# ifdef CPU_COUNT
        /* glibc >= 2.6 has the CPU_COUNT macro.  */
        count = (UInt32)CPU_COUNT (&set);
# else
		size_t i;

		count = 0;
		i = 0;
		while (i < CPU_SETSIZE)
		{
			if (CPU_ISSET (i, &set))
				count++;
			i++;
		}
# endif
        if (count == 0)
			count = 1;
		return count;
	}
	else
	{
		OSInt procSys;
		procSys = sysconf(_SC_NPROCESSORS_CONF);
		if (procSys > 0)
			return (UOSInt)procSys;
		return 1;
	}
#endif
}

Bool Sync::Thread::EnableInterrupt()
{
/*	struct sigaction act;
	MemClear(&act, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigfillset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(SIGINT, &act, 0) == 0;*/
	return false;
}

Bool Sync::Thread::Interrupt(UInt32 threadId)
{
//	return pthread_kill((pthread_t)threadId, SIGINT) == 0;
	return false;
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
