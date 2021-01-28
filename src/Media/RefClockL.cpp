#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/RefClock.h"
#include <sys/time.h>

Media::RefClock::RefClock()
{
	started = false;
}

Media::RefClock::~RefClock()
{
}

void Media::RefClock::Start(Int32 currTime)
{
	struct timeval t;
	struct timezone tz;
	gettimeofday(&t, &tz);
	this->refStart = t.tv_sec * (Int64)1000000 + t.tv_usec;
	this->refStartTime = currTime;
	started = true;

}

void Media::RefClock::Stop()
{
	started = false;
}

Int32 Media::RefClock::GetCurrTime()
{
	if (started)
	{
		Int32 thisTime;
		struct timeval t;
		struct timezone tz;
		gettimeofday(&t, &tz);
		thisTime = this->refStartTime + ((t.tv_sec * (Int64)1000000 + t.tv_usec) - this->refStart) / 1000;
		return thisTime;
	}
	else
	{
		return 0;
	}
}

Bool Media::RefClock::Running()
{
	return started;
}

