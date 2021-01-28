#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "IO/SimpleFileWriter.h"
#if defined(CPU_X86_32) || defined(CPU_X86_64)
#include "Manage/CPUInfo.h"
#endif
#include "Sync/Mutex.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include <stdlib.h>
#include <stdio.h>
#if !defined(__FreeBSD__) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <wchar.h>
#include <signal.h>

Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
OSInt mcBreakPt = 0;
const UTF8Char *mcLogFile = 0;
Sync::MutexData mcMut;

Int32 mcBlockId = 0;

#if defined(CPU_X86_64)
extern "C"
{
	void MemClearANC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_SSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyANC_SSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, OSInt leng);

	void MemClearANC_AVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyANC_AVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, OSInt leng);

	void MemClearANC_AMDSSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDSSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyANC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNAC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNANC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng);

	void MemClearANC_AMDAVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDAVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyANC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNAC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng);
	void MemCopyNANC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng);
}

MemClearFunc MemClearANC = MemClearANC_SSE;
MemClearFunc MemClearAC = MemClearAC_SSE;
MemCopyFunc MemCopyAC = MemCopyAC_SSE;
MemCopyFunc MemCopyANC = MemCopyANC_SSE;
MemCopyFunc MemCopyNAC = MemCopyNAC_SSE;
MemCopyFunc MemCopyNANC = MemCopyNANC_SSE;

extern "C"
{
	Int32 UseAVX = 0;
	Int32 CPUBrand = 0;
}
#elif defined(CPU_X86_32)
extern "C"
{
	Int32 UseAVX = 0;
	Int32 CPUBrand = 0;
}
#endif

void MemPtrChk(void *ptr)
{
	if ((OSInt)ptr == mcBreakPt)
	{
		wprintf(L"Out of Memory\n");
	}
} 

void MemInit()
{
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
		mcBusy = 0;
		mcBreakPt = 0;
		Sync::Mutex_Create(&mcMut);

#if defined(CPU_X86_64)
		Manage::CPUInfo cpuInfo;
		CPUBrand = (Int32)cpuInfo.GetBrand();
		if (CPUBrand == 2)
		{
			if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX2))
			{
				UseAVX = 1;
				MemClearANC = MemClearANC_AMDAVX;
				MemClearAC = MemClearAC_AMDAVX;
				MemCopyAC = MemCopyAC_AMDAVX;
				MemCopyANC = MemCopyANC_AMDAVX;
				MemCopyNAC = MemCopyNAC_AMDAVX;
				MemCopyNANC = MemCopyNANC_AMDAVX;
			}
			else
			{
				MemClearANC = MemClearANC_AMDSSE;
				MemClearAC = MemClearAC_AMDSSE;
				MemCopyAC = MemCopyAC_AMDSSE;
				MemCopyANC = MemCopyANC_AMDSSE;
				MemCopyNAC = MemCopyNAC_AMDSSE;
				MemCopyNANC = MemCopyNANC_AMDSSE;
			}
		}
		else
		{
			if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX) && cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX2))
			{
				UseAVX = 1;
				MemClearANC = MemClearANC_AVX;
				MemClearAC = MemClearAC_AVX;
				MemCopyAC = MemCopyAC_AVX;
				MemCopyANC = MemCopyANC_AVX;
				MemCopyNAC = MemCopyNAC_AVX;
				MemCopyNANC = MemCopyNANC_AVX;
			}
		}
#elif defined(CPU_X86_32)
	Manage::CPUInfo cpuInfo;
	CPUBrand = (Int32)cpuInfo.GetBrand();
	if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX2))
	{
		UseAVX = 1;
	}
#endif
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
		OSInt size = Text::StrCharCnt(logFile);
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

void *MAlloc(OSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	void *ptr = malloc(size + 4);
	if ((OSInt)ptr == 0)
	{
		wprintf(L"Out of Memory: size = %d\n", size);
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

void *MAllocA(OSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 32);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA %x\r\n", mptr);
	if ((OSInt)mptr == 0)
	{
		wprintf(L"Out of Memory: Asize = %d\n", size);
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

void *MAllocA64(OSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)malloc(size + 80);
	UInt8 *sptr = mptr;
//	wprintf(L"MAllocA64 %lx\r\n", mptr);
	if ((OSInt)mptr == 0)
	{
		wprintf(L"Out of Memory: A64size = %d\n", size);
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

IO::IWriter *MemOpenWriter()
{
	if (mcLogFile)
	{
		return new IO::SimpleFileWriter(mcLogFile, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE);
	}
	else
	{
		return new IO::ConsoleWriter();
	}
}


Int32 MemCheckError()
{
	Bool found = false;
	IO::IWriter *console = 0;

	if (mcMemoryCnt)
	{
		UTF8Char buff[12];
		Text::StrInt32(buff, mcMemoryCnt);
		console = MemOpenWriter();
		console->Write((const UTF8Char*)"Memory leaks occurs for ");
		console->Write(buff);
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

