#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt64.h"
#include "IO/StreamWriter.h"
#include "IO/FileStream.h"
#include "Manage/ExceptionLogger.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/StackTracer.h"
#include "Manage/SymbolResolver.h"
#if defined(CPU_X86_32) || defined(CPU_X86_64) || defined(_M_ARM64EC)
#include "Manage/DasmX86_32.h"
#include "Manage/ThreadContextX86_32.h"
#if defined(CPU_X86_64) || defined(_M_ARM64EC)
#include "Manage/DasmX86_64.h"
#include "Manage/ThreadContextX86_64.h"
#endif
#elif defined(CPU_ARM64)
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM64.h"
#elif defined(CPU_ARM)
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM.h"
#endif
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.hpp"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#ifndef _WIN32_WCE
#include <dbghelp.h>
#endif
#include <stdio.h>

#define STACKDUMPSIZE 4096

NN<Text::String> Manage::ExceptionRecorder::fileName;
Manage::ExceptionRecorder::ExceptionAction Manage::ExceptionRecorder::exAction;

#ifndef _WIN32_WCE

typedef LONG    NTSTATUS;
typedef NTSTATUS (WINAPI *pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);
#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)
#define ThreadQuerySetWin32StartAddress 9

DWORD WINAPI GetThreadStartAddress(HANDLE hThread)
{
	NTSTATUS ntStatus;
	HANDLE hDupHandle;
	DWORD dwStartAddress;

	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
	if (NtQueryInformationThread == NULL) return 0;

	HANDLE hCurrentProcess = GetCurrentProcess();

	if (!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0))
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return 0;
	}

	ntStatus = NtQueryInformationThread(hDupHandle, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);
	CloseHandle(hDupHandle);
	if(ntStatus != STATUS_SUCCESS) return 0;

    return dwStartAddress;
}
#endif

Text::CStringNN Manage::ExceptionRecorder::GetExceptionCodeName(UInt32 exCode)
{
	switch (exCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return CSTR("Access Violation");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return CSTR("Array Bounds Exceeded");
	case EXCEPTION_BREAKPOINT:
		return CSTR("Breakpoint");
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return CSTR("Datatype Misalignment");
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return CSTR("Floating point Denormal Operand");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return CSTR("Floating point Device by Zero");
	case EXCEPTION_FLT_INEXACT_RESULT:
		return CSTR("Floating poitn Inexact Result");
	case EXCEPTION_FLT_INVALID_OPERATION:
		return CSTR("Floating point Invalid Operation");
	case EXCEPTION_FLT_OVERFLOW:
		return CSTR("Floating point overflow");
	case EXCEPTION_FLT_STACK_CHECK:
		return CSTR("Floating point stack check error");
	case EXCEPTION_FLT_UNDERFLOW:
		return CSTR("Floating point underflow");
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return CSTR("Illegal Instruction");
	case EXCEPTION_IN_PAGE_ERROR:
		return CSTR("In page error");
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return CSTR("Integer divide by zero");
	case EXCEPTION_INT_OVERFLOW:
		return CSTR("Integer Overflow");
	case EXCEPTION_INVALID_DISPOSITION:
		return CSTR("Invalid Disposition");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return CSTR("Noncontinuable Exception");
	case EXCEPTION_PRIV_INSTRUCTION:
		return CSTR("Priv Instruction");
	case EXCEPTION_SINGLE_STEP:
		return CSTR("Single Step");
	case EXCEPTION_STACK_OVERFLOW:
		return CSTR("Stack Overflow");
	default:
		return CSTR("Unknown Exception");
	}

}

Int32 __stdcall Manage::ExceptionRecorder::ExceptionHandler(void *exInfo)
{
	NN<Manage::ThreadContext> tCont;
	EXCEPTION_POINTERS *info = (EXCEPTION_POINTERS*)exInfo;
	printf("exception occured: %s\r\n", GetExceptionCodeName(info->ExceptionRecord->ExceptionCode).v.Ptr());
#if defined(CPU_X86_64) || defined(_M_ARM64EC)
	NEW_CLASSNN(tCont, Manage::ThreadContextX86_64(GetCurrentProcessId(), 0, info->ContextRecord));
#elif defined(CPU_X86_32)
	NEW_CLASSNN(tCont, Manage::ThreadContextX86_32(GetCurrentProcessId(), 0, info->ContextRecord));
#elif defined(CPU_ARM64)
	NEW_CLASSNN(tCont, Manage::ThreadContextARM64(GetCurrentProcessId(), 0, info->ContextRecord));
#elif defined(CPU_ARM)
	NEW_CLASSNN(tCont, Manage::ThreadContextARM(GetCurrentProcessId(), 0, info->ContextRecord));
#else
#error "Unsupported CPU"
#endif
	Manage::ExceptionLogger::LogToFile(fileName, info->ExceptionRecord->ExceptionCode, GetExceptionCodeName(info->ExceptionRecord->ExceptionCode), (UIntOS)info->ExceptionRecord->ExceptionAddress, tCont);
	tCont.Delete();

	if (exAction == EA_CLOSE)
	{
	}
	else if (info->ExceptionRecord->ExceptionCode == 0x80000003)
	{
#if defined(CPU_X86_64)
		info->ContextRecord->Rip += 1;
#elif defined(CPU_X86_32)
		info->ContextRecord->Eip += 1;
#elif defined(CPU_ARM)
		info->ContextRecord->Pc += 4;
#endif
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	if (Manage::ExceptionRecorder::exAction == EA_CONTINUE)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

Manage::ExceptionRecorder::ExceptionRecorder(Text::CStringNN fileName, ExceptionAction exAction)
{
	Manage::ExceptionRecorder::fileName = Text::String::New(fileName);
	Manage::ExceptionRecorder::exAction = exAction;
#ifndef _WIN32_WCE
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)&ExceptionHandler);
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif
}

Manage::ExceptionRecorder::~ExceptionRecorder()
{
	this->fileName->Release();
}