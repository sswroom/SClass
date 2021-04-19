#include "Stdafx.h"
#include "Manage/HiResClock.h"
#if __cplusplus >= 201112L && !defined(__FreeBSD__) && defined(TIME_UTC)
#include <time.h>

Manage::HiResClock::HiResClock()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	this->stTime = ts.tv_sec * (Int64)1000000000 + ts.tv_nsec;
}

Manage::HiResClock::~HiResClock()
{
}

void Manage::HiResClock::Start()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	this->stTime = ts.tv_sec * (Int64)1000000000 + ts.tv_nsec;
}

Double Manage::HiResClock::GetTimeDiff()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	Int64 t2 = ts.tv_sec * (Int64)1000000000 + ts.tv_nsec;
	if (t2 >= this->stTime)
	{
		return (t2 - this->stTime) * 0.000000001;
	}
	else
	{
		return (86400000000000 - (t2 - this->stTime)) * 0.000000001;
	}
}

Int64 Manage::HiResClock::GetTimeDiffus()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	Int64 t2 = ts.tv_sec * (Int64)1000000000 + ts.tv_nsec;
	if (t2 >= this->stTime)
	{
		return (t2 - this->stTime) / 1000;
	}
	else
	{
		return (86400000000000 - (t2 - this->stTime)) / 1000;
	}
}

Double Manage::HiResClock::GetAndRestart()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	Double t3;
	Int64 t2 = ts.tv_sec * (Int64)1000000000 + ts.tv_nsec;
	if (t2 >= this->stTime)
	{
		t3 = (t2 - this->stTime) * 0.000000001;
	}
	else
	{
		t3 = (86400000000000 - (t2 - this->stTime)) * 0.000000001;
	}
	this->stTime = t2;
	return t3;
}
#else
#include <sys/time.h>

Manage::HiResClock::HiResClock()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	this->stTime = t.tv_sec * (Int64)1000000 + t.tv_usec;
}

Manage::HiResClock::~HiResClock()
{
}

void Manage::HiResClock::Start()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	this->stTime = t.tv_sec * (Int64)1000000 + t.tv_usec;
}

Double Manage::HiResClock::GetTimeDiff()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	Int64 t2 = t.tv_sec * (Int64)1000000 + t.tv_usec;
	if (t2 >= this->stTime)
	{
		return (Double)(t2 - this->stTime) * 0.000001;
	}
	else
	{
		return (Double)(86400000000LL - (t2 - this->stTime)) * 0.000001;
	}
}

Int64 Manage::HiResClock::GetTimeDiffus()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	Int64 t2 = t.tv_sec * (Int64)1000000 + t.tv_usec;
	if (t2 >= this->stTime)
	{
		return (t2 - this->stTime);
	}
	else
	{
		return (86400000000LL - (t2 - this->stTime));
	}
}

Double Manage::HiResClock::GetAndRestart()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	Double t3;
	Int64 t2 = t.tv_sec * (Int64)1000000 + t.tv_usec;
	if (t2 > this->stTime)
	{
		t3 = (Double)(t2 - this->stTime) * 0.000001;
	}
	else
	{
		t3 = (Double)(86400000000LL - (t2 - this->stTime)) * 0.000001;
	}
	this->stTime = t2;
	return t3;
}

Int64 Manage::HiResClock::GetRelTime_us()
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	return t.tv_sec * (Int64)1000000 + t.tv_usec;
}
#endif
