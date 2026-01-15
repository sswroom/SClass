#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/StackTracer.h"
#include "Manage/ThreadContextX86_32.h"
#include "Manage/ThreadContextX86_64.h"
#include "Text/Encoding.h"

Manage::StackTracer::StackTracer(Manage::ThreadContext *context)
{
	this->context = context;
	this->addrArr = 0;
	this->stackFrame = 0;
	NEW_CLASS(this->addrArr, Data::ArrayListInt64());
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
	return this->addrArr->GetItem((IntOS)this->stackFrame);
}

Bool Manage::StackTracer::GoToNextLevel()
{
	if (this->addrArr == 0)
		return false;
	if ((IntOS)this->stackFrame >= this->addrArr->GetCount() - 1)
		return false;
	this->stackFrame = (void*)(1 + (IntOS)this->stackFrame);
	return true;
}

