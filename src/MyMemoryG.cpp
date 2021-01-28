#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Mutex.h"
#include "Sync/Interlocked.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>
#if defined(MSC_VER) && !defined(_WIN32_WCE)
#include <crtdbg.h>
#endif

Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
OSInt mcBreakPt = 0;
Sync::Mutex *mcMut = 0;

void MemPtrChk(void *ptr)
{
#ifdef HAS_ASM32
	if ((OSInt)ptr == mcBreakPt)
	{
		_asm int 3;
	}
#else
	if ((OSInt)ptr == mcBreakPt)
	{
		MessageBoxW(0, L"Out of Memory", L"Error", 0);
	}
#endif
}

void MemStart()
{
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
		mcBusy = 0;
		mcBreakPt = 0;
		mcMut = new Sync::Mutex();
	}
}

void MemSetBreakPoint(Int32 address)
{
	mcBreakPt = address;
}

void MemLock()
{
	mcMut->Lock();
}

void MemUnlock()
{
	mcMut->Unlock();
}

void *MAlloc(OSInt size)
{
	mcMut->Lock();
	mcMemoryCnt++;

	void *ptr = malloc(size);
#ifdef HAS_ASM32
	if ((OSInt)ptr == mcBreakPt)
	{
		_asm int 3;
	}
#else
	if ((OSInt)ptr == mcBreakPt)
	{
		MessageBoxW(0, L"Out of Memory", L"Error", 0);
	}
#endif
	mcMut->Unlock();
	return ptr;
}

void MemFree(void *ptr)
{
	mcMut->Lock();
	mcMemoryCnt--;
//	_heapchk();
	free(ptr);
	mcMut->Unlock();
}

void MemEnd()
{
	if (Sync::Interlocked::Decrement(&mcInitCnt) == 0)
	{
		MemCheckError();
		delete mcMut;
	}
}

Int32 MemCheckError()
{
	Bool found = false;
	if (mcMemoryCnt)
	{
		WChar buff[12];
		Text::StrInt32(buff, mcMemoryCnt);
		IO::ConsoleWriter writer;
		writer.Write(L"Memory leaks occurs for ");
		writer.Write(buff);
		writer.WriteLine(L" times");
		found = true;
	}
#if defined(MSC_VER) && !defined(_WIN32_WCE)
	if (_CrtDumpMemoryLeaks())
		found = true;
#endif
	if (found)
		return 1;
	else
		return 0;
}

Int32 MemCountBlks()
{
	return mcMemoryCnt;
}
