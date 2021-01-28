#include "stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInfo.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
//#include "php.h"
#define emalloc(a) malloc(a)
#define efree(a) free(a)

#define MYMEMSIZE 1677721600

#ifdef HAS_INTRIN
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#endif

#if !defined(HAS_ASM32)
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

extern "C" Int32 UseAVX = 0;
extern "C" Int32 CPUBrand = 0;
#endif

UInt8 *MemMain;
Int32 MemTotal;
Int32 MemAllocated;
Int32 MemAllocCnt;
Int32 MemFreeCnt;
OSInt mcBreakPt = 0;
Int32 mcInitCnt = 0;
Sync::Mutex *mcMut = 0;

typedef struct
{
	Int32 prevBlock;
	Int32 nextBlock;
	Int32 size;
	Int32 flags;
} MemBlock;

void MemPtrChk(void *ptr)
{
	if ((OSInt)ptr == mcBreakPt)
	{
		DebugBreak();
	}
}

void MemInit()
{
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
#if !defined(HAS_ASM32)
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
			if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX))
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
#endif
		MemMain = (UInt8*)emalloc(MYMEMSIZE);
		MemBlock *blk = (MemBlock*)&MemMain[0];
		blk->prevBlock = -1;
		blk->nextBlock = -1;
		blk->size = MYMEMSIZE - 16;
		blk->flags = 0;
		MemTotal = 1;
		MemAllocated = 0;
		MemAllocCnt = 0;
		MemFreeCnt = 0;
		mcMut = new Sync::Mutex();
	}
}

void MemSetLogFile(const WChar *logFile)
{
}

Int32 MemCheckError()
{
	Int32 totalCnt = 0;
	Int32 allocCnt = 0;
	Int32 prevPtr = -1;
	Int32 thisPtr = 0;
	MemBlock *blk = (MemBlock*)&MemMain[0];
	while (MemTotal >= totalCnt)
	{
		if (blk->prevBlock != prevPtr)
			return 1;
		if (blk->flags == 0)
		{
			totalCnt++;
		}
		else if (blk->flags == 1)
		{
			totalCnt++;
			allocCnt++;
		}
		else
		{
			return 1;
		}


		if (blk->nextBlock == -1)
			break;
		if (blk->nextBlock != thisPtr + blk->size + 16)
		{
			return 1;
		}
		prevPtr = thisPtr;
		thisPtr = blk->nextBlock;
		blk = (MemBlock*)&MemMain[thisPtr];
	}
	if (MemTotal != totalCnt)
		return 1;
	if (MemAllocated != allocCnt)
		return 1;
	return 0;
}

void *MAlloc(OSInt size)
{
//	void *ptr = emalloc(size);
//	return ptr;

	MemBlock *blk = (MemBlock*)&MemMain[0];
	MemBlock *nextBlk;
	mcMut->Lock();
	MemAllocCnt++;
	Int32 currPos = 0;;
	if (MemAllocCnt == 0x16d)
	{
		nextBlk = 0;
	}
	if (MemCheckError())
	{
		mcMut->Unlock();
		return 0;
	}
	while (true)
	{
		if (blk->flags == 0)
		{
			if (blk->size > size + 16)
			{
//				if (MemAllocCnt == 0xf6)
//				{
//					nextBlk = 0;
//				}
				nextBlk = (MemBlock*)&MemMain[currPos + size + 16];
				nextBlk->nextBlock = blk->nextBlock;
				nextBlk->prevBlock = currPos;
				nextBlk->flags = 0;
				nextBlk->size = (blk->size - size - 16);
				if (blk->nextBlock > 0)
				{
					nextBlk = (MemBlock*)&MemMain[blk->nextBlock];
					nextBlk->prevBlock = currPos + size + 16;
				}
				blk->nextBlock = currPos + size + 16;
				blk->flags = 1;
				blk->size = size;
				MemTotal++;
				MemAllocated++;
				MemCheckError();
				mcMut->Unlock();
				return &MemMain[currPos + 16];
			}
			else if (blk->size >= size)
			{
				blk->flags = 1;
				MemAllocated++;
				MemCheckError();
				mcMut->Unlock();
				return &MemMain[currPos + 16];
			}
		}
		else if (blk->flags != 1)
		{
			mcMut->Unlock();
			return 0;
		}

		if (blk->nextBlock == -1)
		{
			mcMut->Unlock();
			return 0;
		}
		else if (blk->nextBlock <= 0)
		{
			mcMut->Unlock();
			return 0;
		}
		blk = (MemBlock*)&MemMain[currPos = blk->nextBlock];
	}
	mcMut->Unlock();
	return 0;
}

void *MAllocA64(OSInt size)
{
	UInt8 *mptr = (UInt8*)MAlloc(size + 80);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt)
	{
		DebugBreak();
	}
	mcMut->Unlock();
#if defined(HAS_ASM32)
#if defined(_DEBUG)
	_asm
	{
		mov ebx,mptr
		mov eax,dword ptr [ebp+4]
		mov dword ptr [ebx],eax
	}
#else
	_asm
	{
		mov ebx,mptr
		mov eax,dword ptr [esp+12]
		mov dword ptr [ebx],eax
	}
#endif
#elif defined(HAS_INTRIN)
	*(OSInt*)mptr = (OSInt)_ReturnAddress();
#else
	*(OSInt*)mptr = 0;
#endif

	mptr += 16;
	mptr += 64 - (63 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void MemFree(void *ptr)
{
//	printf("MemFree 0x%X\r\n", ptr);
//	efree(ptr);
	mcMut->Lock();
	MemFreeCnt++;

	if (MemCheckError())
	{
		mcMut->Unlock();
		return;
	}

	if (ptr == 0)
	{
		mcMut->Unlock();
		return;
	}

	MemBlock *blk = (MemBlock*)&((UInt8*)ptr)[-16];
	if (blk->flags != 1)
	{
		mcMut->Unlock();
		return;
	}
	MemBlock *blk2;
	blk->flags = 0;
	MemAllocated--;
	if (blk->prevBlock != -1)
	{
		if (blk->prevBlock < 0)
		{
			mcMut->Unlock();
			return;
		}
		blk2 = (MemBlock*)&MemMain[blk->prevBlock];
		if (blk2->flags == 0)
		{
			blk2->size += blk->size + 16;
			blk2->nextBlock = blk->nextBlock;
			if (blk2->nextBlock > 0)
			{
				blk = (MemBlock*)&MemMain[blk2->nextBlock];
				blk->prevBlock = ((UInt8*)blk2) - MemMain;
			}
			blk = blk2;
			MemTotal--;
		}
		else if (blk2->flags != 1)
		{
			mcMut->Unlock();
			return;
		}
	}

	if (blk->nextBlock != -1)
	{
		if (blk->nextBlock <= 0)
		{
			mcMut->Unlock();
			return;
		}
		blk2 = (MemBlock*)&MemMain[blk->nextBlock];
		if (blk2->flags == 0)
		{
			blk->size += blk2->size + 16;
			blk->nextBlock = blk2->nextBlock;
			if (blk->nextBlock > 0)
			{
				blk2 = (MemBlock*)&MemMain[blk->nextBlock];
				blk2->prevBlock = ((UInt8*)blk) - MemMain;
			}
			MemTotal--;
		}
		else if (blk2->flags != 1)
		{
			mcMut->Unlock();
			return;
		}
	}

	if (MemCheckError())
	{
		mcMut->Unlock();
		return;
	}
	mcMut->Unlock();
}

void MemFreeA(void *ptr)
{
	MemFree(*(UInt8**)&((UInt8*)ptr)[-8]);
}


void MemDeinit()
{
	if (Sync::Interlocked::Decrement(&mcInitCnt) == 0)
	{
		MemCheckError();
		delete mcMut;
		efree(MemMain);
	}
}
