#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include <windows.h>

Manage::HiResClock::HiResClock()
{
	LARGE_INTEGER liSt;
	QueryPerformanceCounter(&liSt);
	this->stTime = liSt.QuadPart;
}

Manage::HiResClock::~HiResClock()
{
}

void Manage::HiResClock::Start()
{
	LARGE_INTEGER liSt;
	QueryPerformanceCounter(&liSt);
	this->stTime = liSt.QuadPart;
}

Double Manage::HiResClock::GetTimeDiff()
{
	LARGE_INTEGER liEd;
	LARGE_INTEGER liFreq;
	QueryPerformanceCounter(&liEd);
	QueryPerformanceFrequency(&liFreq);
	return (liEd.QuadPart - stTime) / (Double)liFreq.QuadPart;
}

Int64 Manage::HiResClock::GetTimeDiffus()
{
	LARGE_INTEGER liEd;
	LARGE_INTEGER liFreq;
	QueryPerformanceCounter(&liEd);
	QueryPerformanceFrequency(&liFreq);
	return (Int64)((liEd.QuadPart - stTime) / (Double)liFreq.QuadPart * 1000000.0);
}

Double Manage::HiResClock::GetAndRestart()
{
	LARGE_INTEGER liEd;
	LARGE_INTEGER liFreq;
	Double t;
	QueryPerformanceCounter(&liEd);
	QueryPerformanceFrequency(&liFreq);
	t = (liEd.QuadPart - this->stTime) / (Double)liFreq.QuadPart;
	this->stTime = liEd.QuadPart;
	return t;
}
