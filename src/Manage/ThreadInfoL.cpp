#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/Process.h"
#if defined(CPU_X86_32)
#include "Manage/ThreadContextX86_32.h"
#elif defined(CPU_X86_64)
#include "Manage/ThreadContextX86_64.h"
#elif defined(CPU_ARM)
#include "Manage/ThreadContextARM.h"
#elif defined(CPU_ARM64)
#include "Manage/ThreadContextARM64.h"
#elif defined(CPU_MIPS)
#include "Manage/ThreadContextMIPS.h"
#endif
#include "Manage/ThreadInfo.h"
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#if defined(__FreeBSD__)
#include <sys/thr.h>
#endif

//#define VERBOSE

void ThreadInfo_Handler(int signum, siginfo_t *info, void *ucontext)
{
#ifdef VERBOSE
	printf("ThreadInfo: Arrive ThreadInfo_Handler\r\n");
#endif
	if (info == 0 || ucontext == 0)
	{
		return;
	}
#if defined(CPU_X86_32)
	Manage::ThreadContextX86_32 context((UIntOS)info->si_pid, 0, ucontext);
#elif defined(CPU_X86_64)
	Manage::ThreadContextX86_64 context((UIntOS)info->si_pid, 0, ucontext);
#elif defined(CPU_ARM)
	Manage::ThreadContextARM context((UIntOS)info->si_pid, 0, ucontext);
#elif defined(CPU_ARM64)
	Manage::ThreadContextARM64 context((UIntOS)info->si_pid, 0, ucontext);
#elif defined(CPU_MIPS)
	Manage::ThreadContextMIPS context((UIntOS)info->si_pid, 0, ucontext);
#else
#error Unsupported architecture.
	return;
#endif	
	Manage::ThreadInfo::OnContext(context, (UIntOS)gettid());
}

Optional<Manage::ThreadInfo> Manage::ThreadInfo::me = nullptr;

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
	this->hdlr = nullptr;
	this->evt = nullptr;
	this->userObj = nullptr;
}

Manage::ThreadInfo::ThreadInfo(UIntOS procId, UIntOS threadId)
{
	this->threadId = threadId;
	this->procId = procId;
	this->hand = nullptr;
	this->hdlr = nullptr;
	this->evt = nullptr;
	this->userObj = nullptr;
}

Manage::ThreadInfo::~ThreadInfo()
{
	this->hand = nullptr;
}

Bool Manage::ThreadInfo::GetThreadContext(ContextHandler hdlr, AnyType userObj)
{
	this->hdlr = hdlr;
	this->userObj = userObj;
	me = this;
	struct sigaction sa;
	MemClear(&sa, sizeof(sa));
	sa.sa_sigaction = ThreadInfo_Handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, 0);
	Sync::Event evt;
	this->evt = evt;
#ifdef VERBOSE
	printf("ThreadInfo: Call pthread_kill %lu\r\n", (unsigned long)this->threadId);
#endif
	if (this->hand.NotNull())
	{
		if (pthread_kill((pthread_t)this->hand.OrNull(), SIGUSR1) != 0)
		{
			this->hdlr = nullptr;
			this->userObj = nullptr;
			this->evt = nullptr;
			me = nullptr;
			sigaction(SIGUSR1, nullptr, 0);
			return false;
		}
	}
	else
	{
		if (tgkill((pid_t)this->procId, (pid_t)this->threadId, SIGUSR1) != 0)
		{
			this->hdlr = nullptr;
			this->userObj = nullptr;
			this->evt = nullptr;
			me = nullptr;
			sigaction(SIGUSR1, nullptr, 0);
			return false;
		}
	}
#ifdef VERBOSE
	printf("ThreadInfo: End Call pthread_kill %lu\r\n", (unsigned long)this->threadId);
#endif
	evt.Wait(1000);
	if (this->hdlr)
	{
		this->hdlr = nullptr;
		this->userObj = nullptr;
		this->evt = nullptr;
		me = nullptr;
		sigaction(SIGUSR1, nullptr, 0);
		return false;
	}
	sigaction(SIGUSR1, nullptr, 0);
	this->evt = nullptr;
	me = nullptr;
	return true;
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

void Manage::ThreadInfo::OnContext(NN<Manage::ThreadContext> context, UIntOS threadId)
{
	NN<Manage::ThreadInfo> info;
	NN<Sync::Event> evt;
#ifdef VERBOSE
	printf("ThreadInfo: OnContext called\n");
#endif
	if (me.SetTo(info))
	{
#ifdef VERBOSE
		printf("ThreadInfo: OnContext comparing thread Id: info->threadId=%u, threadId=%u\n", (UInt32)info->threadId, (UInt32)threadId);
#endif
		if (info->threadId == threadId)
		{
#ifdef VERBOSE
			printf("ThreadInfo: OnContext called for threadId matched\n");
#endif
			if (info->hdlr)
			{
#ifdef VERBOSE
				printf("ThreadInfo: OnContext calling hdlr\n");
#endif
				info->hdlr(context, info->userObj);
				info->hdlr = nullptr;
				info->userObj = nullptr;
				if (info->evt.SetTo(evt))
				{
#ifdef VERBOSE
					printf("ThreadInfo: OnContext setting event\n");
#endif
					evt->Set();
				}
			}
		}
	}
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
