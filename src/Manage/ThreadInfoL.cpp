#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"
#include <pthread.h>
#include <unistd.h>
#if defined(__FreeBSD__)
#include <sys/thr.h>
#endif

Optional<Manage::ThreadContext> Manage::ThreadInfo::GetThreadContextHand(UIntOS threadId, UIntOS procId, Optional<Sync::ThreadHandle> hand)
{
//	Manage::ThreadContext *outContext = 0;
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
	this->hand = nullptr;
}

Manage::ThreadInfo::~ThreadInfo()
{
	this->hand = nullptr;
}

Optional<Manage::ThreadContext> Manage::ThreadInfo::GetThreadContext()
{
/////////////////////////////////////////
//	ptrace(PTRACE_GETREGSET, this->threadId, 0, &addr)
	return nullptr;
}

UInt64 Manage::ThreadInfo::GetStartAddress()
{
	return 0;
}

Bool Manage::ThreadInfo::WaitForThreadExit(UInt32 waitTimeout)
{
	if (this->hand.NotNull())
		return pthread_join((pthread_t)this->hand.OrNull(), 0) == 0;
	return false;
}

UInt32 Manage::ThreadInfo::GetExitCode()
{
	void *code;
	if (this->hand.NotNull() && pthread_join((pthread_t)this->hand.OrNull(), &code) == 0)
	{
		return (UInt32)(IntOS)code;
	}
	return 0;
}


UIntOS Manage::ThreadInfo::GetThreadId()
{
	return this->threadId;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadInfo::GetName(UnsafeArray<UTF8Char> buff)
{
#if defined(__GNUC_PREREQ)
#if __GNUC_PREREQ(2, 12) && !defined(__DEFINED_pid_t) && !defined(__UCLIBC_MAJOR__)
	if (this->hand.NotNull())
	{
		if (pthread_getname_np((pthread_t)this->hand.OrNull(), (char*)buff.Ptr(), 32) == 0)
			return buff + Text::StrCharCnt(buff);
	}
#endif
#endif
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[3];
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/proc/"));
	sptr = Text::StrUIntOS(sptr, this->procId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/task/"));
	sptr = Text::StrUIntOS(sptr, this->threadId);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/stat"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	UIntOS readSize;
	if ((readSize = fs.Read(BYTEARR(sbuff))) > 0)
	{
		readSize = Text::StrSplitP(sarr, 3, Text::PString(sbuff, readSize), ' ');
		if (readSize == 3 && sarr[1].EndsWith(')') && sarr[1].StartsWith('('))
		{
			sarr[1].RemoveChars(1);
			return sarr[1].Substring(1).ConcatTo(buff);
		}
	}
	return nullptr;
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
	return this->threadId == (UIntOS)pthread_self();
#endif
}

Optional<Manage::ThreadInfo> Manage::ThreadInfo::GetCurrThread()
{
	NN<Manage::ThreadInfo> info;
#if defined(__FreeBSD__)
	long tid;
	if (thr_self(&tid) != 0) tid = 0;
	NEW_CLASSNN(info, Manage::ThreadInfo((UIntOS)getpid(), tid, (Sync::ThreadHandle*)tid));
#else
	NEW_CLASSNN(info, Manage::ThreadInfo((UIntOS)getpid(), (UIntOS)pthread_self(), (Sync::ThreadHandle*)pthread_self()));
#endif
	return info;
}
