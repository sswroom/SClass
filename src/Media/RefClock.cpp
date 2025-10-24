#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/RefClock.h"

Media::RefClock::RefClock()
{
	started = false;
}

Media::RefClock::~RefClock()
{
}

void Media::RefClock::Start(Data::Duration currTime)
{
	this->refStart = Data::TimeInstant::Now();
	this->refStartTime = currTime;
	this->started = true;

}

void Media::RefClock::Stop()
{
	started = false;
}

Data::Duration Media::RefClock::GetCurrTime()
{
	if (started)
	{
		Data::TimeInstant thisTime = Data::TimeInstant::Now();
		return thisTime.Diff(this->refStart) - this->refStartTime;
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

