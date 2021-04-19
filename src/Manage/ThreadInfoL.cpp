#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"
#include <pthread.h>
#include <unistd.h>
#if defined(__FreeBSD__)
#include <sys/thr.h>
#endif

Manage::ThreadContext *Manage::ThreadInfo::GetThreadContextHand(UOSInt threadId, UOSInt procId, void *hand)
{
//	Manage::ThreadContext *outContext = 0;
	return 0;
}

Manage::ThreadInfo::ThreadInfo(UOSInt procId, UOSInt threadId, void *hand)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = hand;
}

Manage::ThreadInfo::ThreadInfo(UOSInt procId, UOSInt threadId)
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
/////////////////////////////////////////
//	ptrace(PTRACE_GETREGSET, this->threadId, 0, &addr)
	return 0;
}

UInt64 Manage::ThreadInfo::GetStartAddress()
{
	return 0;
}

Bool Manage::ThreadInfo::WaitForThreadExit(UInt32 waitTimeout)
{
	return pthread_join((pthread_t)this->threadId, 0) == 0;
}

UInt32 Manage::ThreadInfo::GetExitCode()
{
	void *code;
	if (pthread_join((pthread_t)this->threadId, &code) == 0)
	{
		return (UInt32)(OSInt)code;
	}
	return 0;
}


UOSInt Manage::ThreadInfo::GetThreadId()
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
#if defined(__FreeBSD__)
	long tid;
	return thr_self(&tid) == 0 && this->threadId == tid;
#else
	return this->threadId == (UOSInt)pthread_self();
#endif
}

Manage::ThreadInfo *Manage::ThreadInfo::GetCurrThread()
{
	Manage::ThreadInfo *info;
#if defined(__FreeBSD__)
	long tid;
	if (thr_self(&tid) != 0) tid = 0;
	NEW_CLASS(info, Manage::ThreadInfo((UOSInt)getpid(), tid, (void*)-1));
#else
	NEW_CLASS(info, Manage::ThreadInfo((UOSInt)getpid(), (UOSInt)pthread_self(), (void*)-1));
#endif
	return info;
}
