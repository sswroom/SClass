#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/Process.h"
#include "Manage/ThreadInfo.h"
#include "Manage/ThreadContextX86_32.h"
#include "Manage/ThreadContextX86_64.h"
#include "Manage/ThreadContextARM.h"
#include "Manage/ThreadContextARM64.h"
#include "Text/MyStringW.h"
#if defined(__MINGW32__)
#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>

Manage::ThreadContext *Manage::ThreadInfo::GetThreadContextHand(UOSInt threadId, UOSInt procId, void *hand)
{
	Manage::ThreadContext *outContext = 0;
#ifdef _WIN32_WCE
	CONTEXT context;
	context.ContextFlags = (CONTEXT_FULL);
	if (::GetThreadContext((HANDLE)hand, &context))
	{
#if defined(CPU_ARM)
		NEW_CLASS(outContext, Manage::ThreadContextARM(procId, threadId, &context));
#elif defined(_ALPHA_)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(CPU_X86_32)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(_MIPS_)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(_PPC_)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(_MPPC_)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(_IA64_)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#elif defined(SHx)
//		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
#endif
		return outContext;
	}
	else
	{
		return 0;
	}
#else
	
#if defined(CPU_X86_64)
	Manage::Process proc(procId, false);
	Manage::ThreadContext::ContextType ct = proc.GetContextType();
	if (ct == Manage::ThreadContext::ContextType::X86_64)
	{
		CONTEXT context;
		context.ContextFlags = (CONTEXT_ALL);
		if (::GetThreadContext((HANDLE)hand, &context))
		{
			NEW_CLASS(outContext, Manage::ThreadContextX86_64(procId, threadId, &context));
			return outContext;
		}
		else
		{
			return 0;
		}
	}
#if !defined(__CYGWIN__)
	else
	{
		WOW64_CONTEXT context;
		context.ContextFlags = (CONTEXT_ALL);
		if (::Wow64GetThreadContext((HANDLE)hand, &context))
		{
			NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
			return outContext;
		}
		else
		{
			return 0;
		}
	}
#else
	return 0;
#endif
#elif defined(CPU_X86_32)
	CONTEXT context;
	context.ContextFlags = (CONTEXT_ALL);
	if (::GetThreadContext((HANDLE)hand, &context))
	{
		NEW_CLASS(outContext, Manage::ThreadContextX86_32(procId, threadId, &context));
		return outContext;
	}
	else
	{
		return 0;
	}
#elif defined(CPU_ARM64)
#if defined(_M_ARM64EC)
	CONTEXT context;
	context.ContextFlags = (CONTEXT_ALL);
	if (::GetThreadContext((HANDLE)hand, &context))
	{
		NEW_CLASS(outContext, Manage::ThreadContextX86_64(procId, threadId, &context));
		return outContext;
	}
	else
	{
		return 0;
	}
#else
	CONTEXT context;
	context.ContextFlags = (CONTEXT_ALL);
	if (::GetThreadContext((HANDLE)hand, &context))
	{
		NEW_CLASS(outContext, Manage::ThreadContextARM64(procId, threadId, &context));
		return outContext;
	}
	else
	{
		return 0;
	}
#endif
#elif defined(CPU_ARM)
	CONTEXT context;
	context.ContextFlags = (CONTEXT_ALL);
	if (::GetThreadContext((HANDLE)hand, &context))
	{
		NEW_CLASS(outContext, Manage::ThreadContextARM(procId, threadId, &context));
		return outContext;
	}
	else
	{
		return 0;
	}
#endif
#endif
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
#ifdef _WIN32_WCE
	this->hand = (void*)this->threadId;
#else
	this->hand = OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | SYNCHRONIZE, false, (DWORD)threadId);
#endif
}

Manage::ThreadInfo::~ThreadInfo()
{
	if (this->hand)
	{
#ifndef _WIN32_WCE
		CloseHandle(this->hand);
#endif
		this->hand = 0;
	}
}

Manage::ThreadContext *Manage::ThreadInfo::GetThreadContext()
{
	Manage::ThreadContext *outContext;

	if (GetCurrentThreadId() ==  this->threadId)
	{
		return GetThreadContextHand(threadId, procId, this->hand);
	}

	SuspendThread((HANDLE)this->hand);
	outContext = GetThreadContextHand(threadId, procId, this->hand);
	ResumeThread((HANDLE)this->hand);
	return outContext;
}

typedef LONG    NTSTATUS;
typedef NTSTATUS (WINAPI *pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)
#define ThreadQuerySetWin32StartAddress 9

UInt64 Manage::ThreadInfo::GetStartAddress()
{
#ifdef _WIN32_WCE
	return 0;
#else
	NTSTATUS ntStatus;
	UInt64 startAddress;

	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
	if (NtQueryInformationThread == NULL) return 0;

	ntStatus = NtQueryInformationThread((HANDLE)this->hand, ThreadQuerySetWin32StartAddress, &startAddress, sizeof(startAddress), NULL);
	if(ntStatus != STATUS_SUCCESS)
	{
		DWORD dwStartAddress;
		ntStatus = NtQueryInformationThread((HANDLE)this->hand, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(dwStartAddress), NULL);
		if(ntStatus != STATUS_SUCCESS)
		{
			return 0;
		}
		return dwStartAddress;
	}

    return startAddress;
#endif
}

Bool Manage::ThreadInfo::WaitForThreadExit(UInt32 waitTimeout)
{
	UInt32 ret;
	if ((ret = WaitForSingleObject((HANDLE)this->hand, waitTimeout)) == WAIT_TIMEOUT)
	{
		return false;
	}
	if (ret == 0xffffffff)
	{
		ret = GetLastError();
	}
	return true;
}

UInt32 Manage::ThreadInfo::GetExitCode()
{
	DWORD exitCode;
	if (GetExitCodeThread((HANDLE)this->hand, &exitCode))
	{
		return exitCode;
	}
	return 0;
}


UOSInt Manage::ThreadInfo::GetThreadId()
{
	return this->threadId;
}

typedef HRESULT (*GetThreadDescriptionFunc)(HANDLE hThread, PWSTR  *ppszThreadDescription);

UTF8Char *Manage::ThreadInfo::GetName(UTF8Char *buff)
{
	GetThreadDescriptionFunc GetThreadDescription = (GetThreadDescriptionFunc)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetThreadDescription");
	WChar *sbuff;
	if (GetThreadDescription == 0) return 0; 
	HRESULT hres = GetThreadDescription((HANDLE)this->hand, &sbuff);
	if (SUCCEEDED(hres))
	{
		buff = Text::StrWChar_UTF8(buff, sbuff);
		LocalFree(sbuff);
		return buff;
	}
	return 0;
}

Bool Manage::ThreadInfo::Suspend()
{
	return SuspendThread((HANDLE)this->hand) == (DWORD)-1;
}

Bool Manage::ThreadInfo::Resume()
{
	return ResumeThread((HANDLE)this->hand) == (DWORD)-1;
}

Bool Manage::ThreadInfo::IsCurrThread()
{
	return this->threadId == GetCurrentThreadId();
}

Manage::ThreadInfo *Manage::ThreadInfo::GetCurrThread()
{
	Manage::ThreadInfo *info;
	NEW_CLASS(info, Manage::ThreadInfo(GetCurrentProcessId(), GetCurrentThreadId(), GetCurrentThread()));
	return info;
}