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

Manage::StackTracer::StackTracer(Optional<Manage::ThreadContext> context)
{
	this->context = context;
	this->stackFrame = 0;
	void *addrArr[50];
	int cnt;
	NEW_CLASSNN(this->addrArr, Data::ArrayListUInt64());
	cnt = backtrace(addrArr, 50);
	IntOS i = 0;
	while (i < cnt)
	{
		this->addrArr->Add((UIntOS)addrArr[i]);
		i++;
	}
}

Manage::StackTracer::~StackTracer()
{
	this->addrArr.Delete();
}

Bool Manage::StackTracer::IsSupported()
{
	return this->addrArr->GetCount() > 0;
}

UInt64 Manage::StackTracer::GetCurrentAddr()
{
	return this->addrArr->GetItem((UIntOS)this->stackFrame);
}

Bool Manage::StackTracer::GoToNextLevel()
{
	if ((UIntOS)this->stackFrame + 1 >= this->addrArr->GetCount())
		return false;
	this->stackFrame = (void*)(1 + (IntOS)this->stackFrame);
	return true;
}

