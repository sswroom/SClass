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

Text::String *Manage::ExceptionRecorder::fileName;
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

Text::CString Manage::ExceptionRecorder::GetExceptionCodeName(UInt32 exCode)
{
	switch (exCode)
	{
	case 1: //SIGHUP
		return {UTF8STRC("Hangup detected on controlling terminal or death of controlling process")};
	case 2: //SIGINT
		return {UTF8STRC("Interrupt from keypoard")};
	case 3: //SIGQUIT
		return {UTF8STRC("Quit from keyboard")};
	case 4: //SIGILL
		return {UTF8STRC("Illegal Instruction")};
	case 6: //SIGABRT
		return {UTF8STRC("Abort signal")};
	case 8: //SIGFPE
		return {UTF8STRC("Floating point exception")};
	case 9: //SIGKILL
		return {UTF8STRC("Kill signal")};
	case 11: //SIGSEGV
		return {UTF8STRC("Invalid memory reference")};
	case 13: //SIGPIPE
		return {UTF8STRC("Broken pipe: write to pipe with no readers")};
	case 14: //SIGALRM
		return {UTF8STRC("Timer signal from alarm")};
	case 15: //SIGTERM
		return {UTF8STRC("Termination signal")};
	default:
		return {UTF8STRC("Unknown Exception")};
	}
}

Int32 __stdcall Manage::ExceptionRecorder::ExceptionHandler(void *exInfo)
{
	ExInfo *einfo = (ExInfo*)exInfo;
	Manage::ThreadContext *context;
#if defined(CPU_X86_32)
	NEW_CLASS(context, Manage::ThreadContextX86_32(einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_X86_64)
	NEW_CLASS(context, Manage::ThreadContextX86_64((UOSInt)einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_ARM)
	NEW_CLASS(context, Manage::ThreadContextARM(einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_ARM64)
	NEW_CLASS(context, Manage::ThreadContextARM64(einfo->info->si_pid, 0, einfo->ucontext));
#elif defined(CPU_MIPS)
	NEW_CLASS(context, Manage::ThreadContextMIPS(einfo->info->si_pid, 0, einfo->ucontext));
#else
#error Unsupported architecture.
#endif
	if (fileName)
	{
		Manage::ExceptionLogger::LogToFile(fileName, (UInt32)einfo->signum, GetExceptionCodeName((UInt32)einfo->signum), (UOSInt)einfo->info->si_addr, context);
	}
	DEL_CLASS(context);

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

Manage::ExceptionRecorder::ExceptionRecorder(const UTF8Char *fileName, ExceptionAction exAction)
{
	Manage::ExceptionRecorder::fileName = Text::String::NewNotNull(fileName);
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
	this->fileName = 0;

	signal(SIGSEGV, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	signal(SIGILL, SIG_DFL);
}
