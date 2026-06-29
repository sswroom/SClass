#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"

Optional<Manage::ThreadContext> Manage::ThreadInfo::GetThreadContextHand(UIntOS threadId, UIntOS procId, Optional<Sync::ThreadHandle> hand)
{
	return nullptr;
}

Manage::ThreadInfo::ThreadInfo(UIntOS procId, UIntOS threadId, Optional<Sync::ThreadHandle> hand)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = hand;
}

Manage::ThreadInfo::ThreadInfo(UIntOS procId, UIntOS threadId)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = (Sync::ThreadHandle*)this->threadId;
}

Manage::ThreadInfo::~ThreadInfo()
{
	this->hand = nullptr;
}

Optional<Manage::ThreadContext> Manage::ThreadInfo::GetThreadContext()
{
	return nullptr;
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

Optional<Manage::ThreadInfo> Manage::ThreadInfo::GetCurrThread()
{
	return nullptr;
}
