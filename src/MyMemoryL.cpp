#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "IO/DebugTool.h"
#include "IO/SimpleFileWriter.h"
#include "Sync/Mutex.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#if !defined(__FreeBSD__) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <signal.h>

//#define THREADSAFE

Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
IntOS mcBreakPt = 0;
const UTF8Char *mcLogFile = 0;
Sync::MutexData mcMut;

Int32 mcBlockId = 0;

void MemPtrChk(void *ptr)
{
	if ((IntOS)ptr == mcBreakPt)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory");
#else
		printf("Out of Memory\n");
#endif
	}
} 

void MemInit()
{
	if (Sync::Interlocked::IncrementI32(mcInitCnt) == 1)
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

void MemSetLogFile(const UTF8Char *logFile, UIntOS nameLen)
{
	if (mcLogFile)
	{
		free((void*)mcLogFile);
		mcLogFile = 0;
	}
	if (logFile)
	{
		mcLogFile = (const UTF8Char*)malloc((nameLen + 1) * sizeof(UTF8Char));
		Text::StrConcatC((UTF8Char*)mcLogFile, logFile, nameLen);
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

#if defined(THREADSAFE)
void *MAlloc(UIntOS size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	void *ptr = malloc(size + 4);
	if ((IntOS)ptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory: size = %d", (UInt32)size);
#else
		printf("Out of Memory: size = %d\n", (UInt32)size);
#endif
		Sync::Mutex_Unlock(&mcMut);
		return 0;
	}
	Int32 blkId = mcBlockId++;
	*(Int32*)ptr = blkId;
//		wprintf(L"MAlloc %d %lx\r\n", blkId, ptr);
	Sync::Mutex_Unlock(&mcMut);
	return &((UInt8*)ptr)[4];
}

void *MAllocA(UIntOS size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 32);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA %x\r\n", mptr);
	if ((IntOS)mptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory: Asize = %d", (UInt32)size);
#else
		printf("Out of Memory: Asize = %d\n", (UInt32)size);
#endif
		Sync::Mutex_Unlock(&mcMut);
		return 0;
	}
	Sync::Mutex_Unlock(&mcMut);
	mptr += 16;
	mptr += 16 - (15 & (IntOS)mptr);
	*(UInt8**)&mptr[-8] = sptr;
//	wprintf(L"MAllocA ret %x\r\n", mptr);
	return mptr;
}

void *MAllocA64(UIntOS size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 80);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA64 %lx\r\n", mptr);
	if ((IntOS)mptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of A64Memory: size = %lld", (UInt64)size);
#else
		printf("Out of Memory: A64size = %lld\n", (UInt64)size);
#endif
		Sync::Mutex_Unlock(&mcMut);
		return 0;
	}
	Sync::Mutex_Unlock(&mcMut);
	mptr += 16;
	mptr += 64 - (63 & (IntOS)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void MemFree(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;
	ptr = &((UInt8*)ptr)[-4];
/*	if (((IntOS)ptr) & 0xf != 0)
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
#else
void *MAlloc(UIntOS size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	REGVAR void *ptr = malloc(size + 4);
	if ((IntOS)ptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory: size = %d", (UInt32)size);
#else
		printf("Out of Memory: size = %d\n", (UInt32)size);
		IO::DebugTool::PrintStackTrace();
#endif
		return 0;
	}
	*(Int32*)ptr = Interlocked_IncrementI32(&mcBlockId);
	return &((UInt8*)ptr)[4];
}

void *MAllocA(UIntOS size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	REGVAR UInt8 *mptr = (UInt8*)malloc(size + 32);
	REGVAR UInt8 *sptr = mptr;
	if ((IntOS)mptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory: Asize = %d", (UInt32)size);
#else
		printf("Out of Memory: Asize = %d\n", (UInt32)size);
		IO::DebugTool::PrintStackTrace();
#endif
		return 0;
	}
	*(Int32*)mptr = Interlocked_IncrementI32(&mcBlockId);
	mptr += 16;
	mptr += 16 - (15 & (IntOS)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void *MAllocA64(UIntOS size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	REGVAR UInt8 *mptr = (UInt8*)malloc(size + 80);
	REGVAR UInt8 *sptr = mptr;
	if ((IntOS)mptr == 0)
	{
#if defined(DEBUGCON)
		syslog(LOG_DEBUG, "Out of Memory: A64size = %lld", (UInt64)size);
#else
		printf("Out of Memory: A64size = %lld\n", (UInt64)size);
		IO::DebugTool::PrintStackTrace();
#endif
		return 0;
	}
	*(Int32*)mptr = Interlocked_IncrementI32(&mcBlockId);
	mptr += 16;
	mptr += 64 - (63 & (IntOS)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void MemFree(void *ptr)
{
	Interlocked_DecrementI32(&mcMemoryCnt);
	ptr = &((UInt8*)ptr)[-4];
	free(ptr);
}

void MemFreeA(void *ptr)
{
	Interlocked_DecrementI32(&mcMemoryCnt);
	free(*(UInt8**)&((UInt8*)ptr)[-8]);
}
#endif
void MemDeinit()
{
	if (Sync::Interlocked::DecrementI32(mcInitCnt) == 0)
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
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrInt32(buff, mcMemoryCnt);
		console = MemOpenWriter();
		console->Write(CSTR("Memory leaks occurs for "));
		console->Write(CSTRP(buff, sptr));
		console->WriteLine(CSTR(" times"));
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

