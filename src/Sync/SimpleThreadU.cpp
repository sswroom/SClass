#include "Stdafx.h"
#include "Sync/SimpleThread.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void Sync::SimpleThread::Sleep(UIntOS ms)
{
	UIntOS s = ms / 1000;
	ms = ms % 1000;
	if (s)
		if (!sleep((UInt32)s))
			return;
	if (ms)
		if (!usleep((useconds_t)(ms * 1000)))
			return;
}

void Sync::SimpleThread::Sleepus(UIntOS us)
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
