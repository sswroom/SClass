#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/ExceptionRecorder.h"
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
#include "Text/MyString.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

NN<Text::String> Manage::ExceptionRecorder::fileName;
Manage::ExceptionRecorder::ExceptionAction Manage::ExceptionRecorder::exAction;
Int32 (__stdcall *ExceptionRecorder_Handler)(void *);

typedef struct
{
	int signum;
	siginfo_t *info;
	void *ucontext;
} ExInfo;

void ExceptionRecorder_Signal(int signum, siginfo_t *info, void *ucontext)
{
	ExInfo exInfo;
	exInfo.signum = signum;
	exInfo.info = info;
	exInfo.ucontext = ucontext;
	ExceptionRecorder_Handler(&exInfo);
}

Text::CStringNN Manage::ExceptionRecorder::GetExceptionCodeName(UInt32 exCode)
{
	switch (exCode)
	{
	case 1: //SIGHUP
		return CSTR("Hangup detected on controlling terminal or death of controlling process");
	case 2: //SIGINT
		return CSTR("Interrupt from keypoard");
	case 3: //SIGQUIT
		return CSTR("Quit from keyboard");
	case 4: //SIGILL
		return CSTR("Illegal Instruction");
	case 6: //SIGABRT
		return CSTR("Abort signal");
	case 8: //SIGFPE
		return CSTR("Floating point exception");
	case 9: //SIGKILL
		return CSTR("Kill signal");
	case 11: //SIGSEGV
		return CSTR("Invalid memory reference");
	case 13: //SIGPIPE
		return CSTR("Broken pipe: write to pipe with no readers");
	case 14: //SIGALRM
		return CSTR("Timer signal from alarm");
	case 15: //SIGTERM
		return CSTR("Termination signal");
	default:
		return CSTR("Unknown Exception");
	}
}

Int32 __stdcall Manage::ExceptionRecorder::ExceptionHandler(void *exInfo)
{
	ExInfo *einfo = (ExInfo*)exInfo;
	NN<Manage::ThreadContext> context;
#if defined(CPU_X86_32)
	NEW_CLASSNN(context, Manage::ThreadContextX86_32((UIntOS)einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_X86_64)
	NEW_CLASSNN(context, Manage::ThreadContextX86_64((UIntOS)einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_ARM)
	NEW_CLASSNN(context, Manage::ThreadContextARM(einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_ARM64)
	NEW_CLASSNN(context, Manage::ThreadContextARM64((UIntOS)einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_MIPS)
	NEW_CLASSNN(context, Manage::ThreadContextMIPS(einfo->info->si_pid, 0, einfo->ucontext));
#else
#error Unsupported architecture.
#endif
	Manage::ExceptionLogger::LogToFile(fileName, (UInt32)einfo->signum, GetExceptionCodeName((UInt32)einfo->signum), (UIntOS)einfo->info->si_addr, context);
	context.Delete();

	if (exAction == Manage::ExceptionRecorder::EA_CONTINUE)
	{
	}
	else if (exAction == Manage::ExceptionRecorder::EA_CLOSE)
	{
		exit(-1);
	}
	else if (exAction == Manage::ExceptionRecorder::EA_RESTART)
	{
		UTF8Char sbuff[512];
		IO::Path::GetProcessFileName(sbuff);
		Manage::Process proc(sbuff);
		exit(-1);
	}
	return 0;
}

Manage::ExceptionRecorder::ExceptionRecorder(Text::CStringNN fileName, ExceptionAction exAction)
{
	Manage::ExceptionRecorder::fileName = Text::String::New(fileName);
	Manage::ExceptionRecorder::exAction = exAction;
	ExceptionRecorder_Handler = ExceptionHandler;

	struct sigaction sigact;
	MemClear(&sigact, sizeof(sigact));
	sigact.sa_sigaction = ExceptionRecorder_Signal;
	sigact.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sigact, 0);
	sigaction(SIGFPE, &sigact, 0);
	sigaction(SIGILL, &sigact, 0);
	//signal(SIGSEGV, ExceptionRecorder_Signal);
}

Manage::ExceptionRecorder::~ExceptionRecorder()
{
	this->fileName->Release();

	signal(SIGSEGV, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGILL, SIG_DFL);
}
