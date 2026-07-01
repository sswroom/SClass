#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/StackTracer.h"
#include "Manage/ThreadContextX86_32.h"
#include "Manage/ThreadContextX86_64.h"
#include "Text/Encoding.h"

Manage::StackTracer::StackTracer(Optional<Manage::ThreadContext> context)
{
	this->context = context;
	this->stackFrame = 0;
	NEW_CLASSNN(this->addrArr, Data::ArrayListUInt64());
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
	if ((UIntOS)this->stackFrame >= this->addrArr->GetCount() - 1)
		return false;
	this->stackFrame = (void*)(1 + (UIntOS)this->stackFrame);
	return true;
}

