#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Manage/StackTracer.h"
#include "Manage/ThreadContextX86_32.h"
#include "Manage/ThreadContextX86_64.h"
#include "Manage/ThreadContextARM.h"
#include "Manage/ThreadContextARM64.h"
#include "Text/Encoding.h"
#include <windows.h>

#if defined(_WIN32_WCE)

#define MAX_FRAMES 40
#define STACKSNAP_EXTENDED_INFO 2
typedef ULONG (*GETTHREADCALLSTACK)(HANDLE hThrd, ULONG dwMaxFrames, LPVOID lpFrames[], DWORD dwFlags, DWORD dwSkip);
typedef struct _CallSnapshotEx {
  DWORD dwReturnAddr;
  DWORD dwFramePtr;
  DWORD dwCurProc;
  DWORD dwParams[4];
} CallSnapshotEx;

Manage::StackTracer::StackTracer(Manage::ThreadContext *context)
{
	IO::Library lib((const UTF8Char*)"coredll.dll");
	this->context = context;
	this->addrArr = 0;
	this->stackFrame = 0;
	GETTHREADCALLSTACK pGetThreadCallStack = (GETTHREADCALLSTACK)lib.GetFunc("GetThreadCallStack");
	if (pGetThreadCallStack)
	{
		CallSnapshotEx  lpFrames[MAX_FRAMES];
		DWORD dwCnt = pGetThreadCallStack((HANDLE)context->GetThreadId(), MAX_FRAMES, (void**)lpFrames, STACKSNAP_EXTENDED_INFO, 0);
		NEW_CLASS(this->addrArr, Data::ArrayListInt64());
		DWORD i = 0;
		while (i < dwCnt)
		{
			this->addrArr->Add(lpFrames[i].dwReturnAddr);
			i++;
		}
	}
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
	return 0;
}

Bool Manage::StackTracer::GoToNextLevel()
{
	return false;
}
#else
#include <dbghelp.h>

Manage::StackTracer::StackTracer(Optional<Manage::ThreadContext> context)
{
	this->context = context;
	this->winContext = 0;
	NEW_CLASS(addrArr, Data::ArrayListUInt64());
	STACKFRAME64 *sf = MemAlloc(STACKFRAME64, 1);
	this->stackFrame = sf;
	ZeroMemory(sf, sizeof(STACKFRAME64));
	sf->AddrPC.Mode = AddrModeFlat;
	sf->AddrStack.Mode = AddrModeFlat;
	sf->AddrFrame.Mode = AddrModeFlat;
	NN<Manage::ThreadContext> nncontext;
	if (context.SetTo(nncontext))
	{
		this->winContext = nncontext->Clone().Ptr();
		hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, (DWORD)winContext->GetProcessId());
		hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, false, (DWORD)winContext->GetThreadId());
		sf->AddrPC.Offset = this->winContext->GetInstAddr();
		sf->AddrStack.Offset = this->winContext->GetStackAddr();
		sf->AddrFrame.Offset = this->winContext->GetFrameAddr();
	}
	else
	{
		this->winContext = 0;
		hProc = 0;
		hThread = 0;
		sf->AddrPC.Offset = 0;
		sf->AddrStack.Offset = 0;
		sf->AddrFrame.Offset = 0;
	}
}

Manage::StackTracer::~StackTracer()
{
	SDEL_CLASS(this->winContext);
	if (this->stackFrame)
	{
		MemFree(this->stackFrame);
		this->stackFrame = 0;
	}
	if (hProc)
	{
		CloseHandle(hProc);
		hProc = 0;
	}
	if (hThread)
	{
		CloseHandle(hThread);
		hThread = 0;
	}
	DEL_CLASS(addrArr);
}

Bool Manage::StackTracer::IsSupported()
{
	if (this->winContext)
		return true;
	return false;
}

UInt64 Manage::StackTracer::GetCurrentAddr()
{
	if (this->winContext == 0)
		return 0;
	return this->winContext->GetInstAddr();
}

Bool Manage::StackTracer::GoToNextLevel()
{
	if (this->winContext == 0)
		return 0;

	BOOL ret = 0;
	STACKFRAME64 *sf = (STACKFRAME64*)this->stackFrame;
#if defined(CPU_X86_32) || defined(CPU_X86_64) || defined(_M_ARM64EC)
	if (this->winContext->GetType() == Manage::ThreadContext::ContextType::X86_32)
	{
		ret = StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, hThread, sf, ((Manage::ThreadContextX86_32*)this->winContext)->GetContext(), 0, &SymFunctionTableAccess64, &SymGetModuleBase64, 0);
	}
#endif
#if defined(CPU_X86_64) || defined(_M_ARM64EC)
	else if (this->winContext->GetType() == Manage::ThreadContext::ContextType::X86_64)
	{
		ret = StackWalk64(IMAGE_FILE_MACHINE_AMD64, hProc, hThread, sf, ((Manage::ThreadContextX86_64*)this->winContext)->GetContext(), 0, &SymFunctionTableAccess64, &SymGetModuleBase64, 0);
	}
#endif
#if defined(CPU_ARM64) && !defined(_M_ARM64EC)
	if (this->winContext->GetType() == Manage::ThreadContext::ContextType::ARM64)
	{
		ret = StackWalk64(IMAGE_FILE_MACHINE_ARM64, hProc, hThread, sf, ((Manage::ThreadContextARM64*)this->winContext)->GetContext(), 0, &SymFunctionTableAccess64, &SymGetModuleBase64, 0);
	}
#endif

	if (ret)
	{
		if (this->addrArr->SortedIndexOf(sf->AddrPC.Offset) >= 0)
			return false;
		this->addrArr->SortedInsert(sf->AddrPC.Offset);
		this->winContext->SetInstAddr((UIntOS)sf->AddrPC.Offset);
		this->winContext->SetStackAddr((UIntOS)sf->AddrStack.Offset);
		this->winContext->SetFrameAddr((UIntOS)sf->AddrFrame.Offset);
		return true;
	}
	else
	{
		return false;
	}

}
#endif