#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include <stdlib.h>

Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
OSInt mcBreakPt = 0;
const WChar *mcLogFile = 0;
Sync::MutexData mcMut;

Int32 mcBlockId = 0;

void MemPtrChk(void *ptr)
{
}

void MemInit()
{
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
		mcBusy = 0;
		mcBreakPt = 0;
		Sync::Mutex_Create(&mcMut);
	}
}

void MemSetBreakPoint(Int32 address)
{
	mcBreakPt = address;
}

void MemSetLogFile(const WChar *logFile)
{
}

void MemLock()
{
}

void MemUnlock()
{
}

void *MAlloc(OSInt cnt)
{
	return malloc(cnt);
}

void MemFree(void *p)
{
	free(p);
}

void MemDeinit()
{
	if (Sync::Interlocked::Decrement(&mcInitCnt) == 0)
	{
		MemCheckError();
		Sync::Mutex_Destroy(&mcMut);
	}
}


Int32 MemCheckError()
{
	Bool found = false;
	if (found)
		return 1;
	else
		return 0;
}

Int32 MemCountBlks()
{
	return mcMemoryCnt;
}

#if defined(AVR)
extern "C" void MemCopyNO(void *destPtr, const void *srcPtr, OSInt len)
{
	Char *dPtr = (Char*)destPtr;
	const Char *sPtr = (const Char*)srcPtr;
	while (len-- > 0)
	{
		*dPtr++ = *sPtr++;
	}
}

extern "C" void MemClear(void *buff, OSInt count)
{
	Char *p = (Char*)buff;
	while (count-- > 0)
	{
		*p++ = 0;
	}
}

void *operator new(UOSInt size)
{
	return MAlloc(size);
}

void operator delete(void *p)
{
	MemFree(p);
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}
#endif
