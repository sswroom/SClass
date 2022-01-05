#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "IO/SimpleFileWriter.h"
#include "Sync/Mutex.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include <stdlib.h>
#include <stdio.h>
#if !defined(__FreeBSD__) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <signal.h>

Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
OSInt mcBreakPt = 0;
const UTF8Char *mcLogFile = 0;
Sync::MutexData mcMut;

Int32 mcBlockId = 0;

void MemPtrChk(void *ptr)
{
	if ((OSInt)ptr == mcBreakPt)
	{
		printf("Out of Memory\n");
	}
} 

void MemInit()
{
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
		mcBusy = 0;
		mcBreakPt = 0;
		Sync::Mutex_Create(&mcMut);
		MemTool_Init();
	}
}

void MemSetBreakPoint(Int32 address)
{
	mcBreakPt = address;
}

void MemSetLogFile(const UTF8Char *logFile)
{
	if (mcLogFile)
	{
		free((void*)mcLogFile);
		mcLogFile = 0;
	}
	if (logFile)
	{
		UOSInt size = Text::StrCharCnt(logFile);
		mcLogFile = (const UTF8Char *)malloc((size + 1) * sizeof(UTF8Char));
		Text::StrConcat((UTF8Char*)mcLogFile, logFile);
	}
}

void MemLock()
{
	Sync::Mutex_Lock(&mcMut);
}

void MemUnlock()
{
	Sync::Mutex_Unlock(&mcMut);
}

void *MAlloc(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	void *ptr = malloc(size + 4);
	if ((OSInt)ptr == 0)
	{
		printf("Out of Memory: size = %d\n", (UInt32)size);
	}
	else
	{
		Int32 blkId = mcBlockId++;
		*(Int32*)ptr = blkId;
//		wprintf(L"MAlloc %d %lx\r\n", blkId, ptr);
	}
	Sync::Mutex_Unlock(&mcMut);
	if (ptr)
	{
		return &((UInt8*)ptr)[4];
	}
	else
	{
		return 0;
	}
}

void *MAllocA(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 32);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA %x\r\n", mptr);
	if ((OSInt)mptr == 0)
	{
		printf("Out of Memory: Asize = %d\n", (UInt32)size);
		Sync::Mutex_Unlock(&mcMut);
		return 0;
	}
	Sync::Mutex_Unlock(&mcMut);
	mptr += 16;
	mptr += 16 - (15 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
//	wprintf(L"MAllocA ret %x\r\n", mptr);
	return mptr;
}

void *MAllocA64(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 80);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA64 %lx\r\n", mptr);
	if ((OSInt)mptr == 0)
	{
		printf("Out of Memory: A64size = %d\n", (UInt32)size);
		Sync::Mutex_Unlock(&mcMut);
		return 0;
	}
	Sync::Mutex_Unlock(&mcMut);
	mptr += 16;
	mptr += 64 - (63 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void MemFree(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;
	ptr = &((UInt8*)ptr)[-4];
/*	if (((OSInt)ptr) & 0xf != 0)
	{
		raise(SIGABRT);
	}*/
//	wprintf(L"MemFree %d %lx\r\n", *(Int32*)ptr, ptr);
	free(ptr);
	Sync::Mutex_Unlock(&mcMut);
}

void MemFreeA(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;
//	wprintf(L"MemFreeA %x, Act = %x\r\n", ptr, *(UInt8**)&((UInt8*)ptr)[-8]);
	free(*(UInt8**)&((UInt8*)ptr)[-8]);
	Sync::Mutex_Unlock(&mcMut);
}

void MemDeinit()
{
	if (Sync::Interlocked::Decrement(&mcInitCnt) == 0)
	{
		MemCheckError();
		if (mcLogFile)
		{
			free((void*)mcLogFile);
			mcLogFile = 0;
		}
		Sync::Mutex_Destroy(&mcMut);
	}
}

IO::Writer *MemOpenWriter()
{
	if (mcLogFile)
	{
		return new IO::SimpleFileWriter(mcLogFile, IO::FileMode::Append, IO::FileShare::DenyNone);
	}
	else
	{
		return new IO::ConsoleWriter();
	}
}


Int32 MemCheckError()
{
	Bool found = false;
	IO::Writer *console = 0;

	if (mcMemoryCnt)
	{
		UTF8Char buff[12];
		UTF8Char *sptr;
		sptr = Text::StrInt32(buff, mcMemoryCnt);
		console = MemOpenWriter();
		console->WriteStrC(UTF8STRC("Memory leaks occurs for "));
		console->WriteStrC(buff, (UOSInt)(sptr - buff));
		console->WriteLine((const UTF8Char*)" times");
		delete console;
		found = true;
	}
	if (found)
		return 1;
	else
		return 0;
}

Int32 MemCountBlks()
{
	return mcMemoryCnt;
}

