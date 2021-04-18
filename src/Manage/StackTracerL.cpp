#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/StackTracer.h"
#if defined(NO_STACKTRACE) || defined(__ANDROID__) || defined(__FreeBSD__) || defined(__APPLE__) || defined(__sun__)
int backtrace(void **addrArr, int cnt)
{
	return 0;
}
#else
#include <features.h>
#include <execinfo.h>
#endif

Manage::StackTracer::StackTracer(Manage::ThreadContext *context)
{
	this->context = context;
	this->addrArr = 0;
	this->stackFrame = 0;
	void *addrArr[50];
	int cnt;
	NEW_CLASS(this->addrArr, Data::ArrayListUInt64());
	cnt = backtrace(addrArr, 50);
	OSInt i = 0;
	while (i < cnt)
	{
		this->addrArr->Add((UOSInt)addrArr[i]);
		i++;
	}
}

Manage::StackTracer::~StackTracer()
{
	SDEL_CLASS(this->addrArr);
}

Bool Manage::StackTracer::IsSupported()
{
	return this->addrArr != 0 && this->addrArr->GetCount() > 0;
}

UInt64 Manage::StackTracer::GetCurrentAddr()
{
	if (this->addrArr == 0)
		return 0;
	return this->addrArr->GetItem((UOSInt)this->stackFrame);
}

Bool Manage::StackTracer::GoToNextLevel()
{
	if (this->addrArr == 0)
		return false;
	if ((UOSInt)this->stackFrame >= this->addrArr->GetCount() - 1)
		return false;
	this->stackFrame = (void*)(1 + (OSInt)this->stackFrame);
	return true;
}

