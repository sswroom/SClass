#include "Stdafx.h"
#include <windows.h>
#include "Media/RefClock.h"
#include "Math/Math.h"

Media::RefClock::RefClock()
{
	started = false;
}

Media::RefClock::~RefClock()
{
}

void Media::RefClock::Start(UInt32 currTime)
{
	LARGE_INTEGER liClk;
	QueryPerformanceCounter(&liClk);
	this->refStart = liClk.QuadPart;
	this->refStartTime = currTime;
	started = true;

}

void Media::RefClock::Stop()
{
	started = false;
}

UInt32 Media::RefClock::GetCurrTime()
{
	if (started)
	{
		UInt32 thisTime;
		LARGE_INTEGER liClk;
		LARGE_INTEGER liFreq;

		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liClk);
		thisTime = this->refStartTime + (UInt32)(Double2Int32((Double)(liClk.QuadPart - refStart) * 1000.0 / (Double)liFreq.QuadPart));
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
