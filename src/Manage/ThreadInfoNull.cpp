#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"

Manage::ThreadContext *Manage::ThreadInfo::GetThreadContextHand(UIntOS threadId, UIntOS procId, void *hand)
{
	Manage::ThreadContext *outContext = 0;
	return 0;
}

Manage::ThreadInfo::ThreadInfo(UIntOS procId, UIntOS threadId, void *hand)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = hand;
}

Manage::ThreadInfo::ThreadInfo(UIntOS procId, UIntOS threadId)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = (void*)this->threadId;
}

Manage::ThreadInfo::~ThreadInfo()
{
	if (this->hand)
	{
		this->hand = 0;
	}
}

Manage::ThreadContext *Manage::ThreadInfo::GetThreadContext()
{
	return 0;
}

UInt64 Manage::ThreadInfo::GetStartAddress()
{
	return 0;
}

Bool Manage::ThreadInfo::WaitForThreadExit(UInt32 waitTimeout)
{
	return false;
}

UInt32 Manage::ThreadInfo::GetExitCode()
{
	return 0;
}


UIntOS Manage::ThreadInfo::GetThreadId()
{
	return this->threadId;
}

Bool Manage::ThreadInfo::Suspend()
{
	return false;
}

Bool Manage::ThreadInfo::Resume()
{
	return false;
}

Bool Manage::ThreadInfo::IsCurrThread()
{
	return true;
}

Manage::ThreadInfo *Manage::ThreadInfo::GetCurrThread()
{
	return 0;
}
