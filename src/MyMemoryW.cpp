#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#if !defined(_CONSOLE) && !defined(__CYGWIN__)
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "IO/SimpleFileWriter.h"
#include "Sync/Mutex.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include <windows.h>
#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)) && !defined(_WIN32_WCE)
#include <dbghelp.h>
#endif

//#define THREADSAFE

#if defined(__CYGWIN__) || defined(__MINGW32__)
#define _ReturnAddress() __builtin_return_address(0)
#elif defined(HAS_INTRIN)
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#endif

HANDLE mcIntHandle = 0;
HANDLE mcHandle = 0;
Int32 mcMemoryCnt = 0;
Int32 mcInitCnt = 0;
Int32 mcBusy = 0;
OSInt mcBreakPt = 0;
UOSInt mcBreakSize = 0;
Sync::MutexData mcMut;
const UTF8Char *mcLogFile = 0;

void MemPtrChk(void *ptr)
{
	if ((OSInt)ptr == mcBreakPt)
	{
		DebugBreak();
	}
}

void MemInit()
{
	if (Sync::Interlocked::IncrementI32(mcInitCnt) == 1)
	{
		mcHandle = HeapCreate(0, 0, 0);//GetProcessHeap();
		mcIntHandle = HeapCreate(0, 0, 0);
		mcBusy = 0;
		mcBreakPt = 0;
		mcBreakSize = 0;
		Sync::Mutex_Create(&mcMut);
		MemTool_Init();
	}
}

void MemSetBreakPoint(OSInt address)
{
	mcBreakPt = address;
	mcBreakSize = 0;
}

void MemSetBreakPoint(OSInt address, UOSInt size)
{
	mcBreakPt = address;
	mcBreakSize = size;
}

void MemSetLogFile(const UTF8Char *logFile, UOSInt nameLen)
{
	if (mcLogFile)
	{
		HeapFree(mcIntHandle, 0, (void*)mcLogFile);
		mcLogFile = 0;
	}
	if (logFile)
	{
		mcLogFile = (const UTF8Char*)HeapAlloc(mcIntHandle, 0, (nameLen + 1) * sizeof(UTF8Char));
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
void *MAlloc(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	void *mptr = HeapAlloc(mcHandle, 0, size + 8);
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size + 8))
	{
		DebugBreak();
	}
	Sync::Mutex_Unlock(&mcMut);
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

	return ((UInt8*)mptr) + 8;
}

void *MAllocA(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 32);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
	{
		DebugBreak();
	}
	Sync::Mutex_Unlock(&mcMut);
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
	mptr += 16 - (15 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void *MAllocA64(UOSInt size)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 80);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
	{
		DebugBreak();
	}
	Sync::Mutex_Unlock(&mcMut);
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
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;

//	_heapchk();
	HeapFree(mcHandle, 0, ((UInt8*)ptr) - 8);
	Sync::Mutex_Unlock(&mcMut);
}

void MemFreeA(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;

//	_heapchk();
	UInt8 *relPtr = *(UInt8**)&((UInt8*)ptr)[-8];
#ifdef _DEBUG
	OSInt v = ((UInt8*)ptr) - relPtr;
	if (v > 80 || v < 16)
	{
		v = 0;
	}
#if defined(HAS_INTRIN)
	*(UInt8**)&((UInt8*)ptr)[-8] = (UInt8 *)_ReturnAddress();
#else
	*(UInt8**)&((UInt8*)ptr)[-8] = 0;
#endif
#endif

	HeapFree(mcHandle, 0, relPtr);
	Sync::Mutex_Unlock(&mcMut);
}
#else
void *MAlloc(UOSInt size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	void *mptr = HeapAlloc(mcHandle, 0, size + 8);
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size + 8))
	{
		DebugBreak();
	}
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

	return ((UInt8*)mptr) + 8;
}

void *MAllocA(UOSInt size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 32);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
	{
		DebugBreak();
	}
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
	mptr += 16 - (15 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void *MAllocA64(UOSInt size)
{
	Interlocked_IncrementI32(&mcMemoryCnt);

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 80);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
	{
		DebugBreak();
	}
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
	Interlocked_DecrementI32(&mcMemoryCnt);

//	_heapchk();
	HeapFree(mcHandle, 0, ((UInt8*)ptr) - 8);
}

void MemFreeA(void *ptr)
{
	Interlocked_DecrementI32(&mcMemoryCnt);

//	_heapchk();
	UInt8 *relPtr = *(UInt8**)&((UInt8*)ptr)[-8];
#ifdef _DEBUG
	OSInt v = ((UInt8*)ptr) - relPtr;
	if (v > 80 || v < 16)
	{
		v = 0;
	}
#if defined(HAS_INTRIN)
	*(UInt8**)&((UInt8*)ptr)[-8] = (UInt8 *)_ReturnAddress();
#else
	*(UInt8**)&((UInt8*)ptr)[-8] = 0;
#endif
#endif
	HeapFree(mcHandle, 0, relPtr);
}
#endif
void MemDeinit()
{
	if (Sync::Interlocked::DecrementI32(mcInitCnt) == 0)
	{
		MemCheckError();
		Sync::Mutex_Destroy(&mcMut);
		if (mcLogFile)
		{
			HeapFree(mcIntHandle, 0, (void*)mcLogFile);
			mcLogFile = 0;
		}
		HeapDestroy(mcHandle);
		HeapDestroy(mcIntHandle);
		mcHandle = 0;
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
#if !defined(_CONSOLE) && !defined(__CYGWIN__)
		return new IO::DebugWriter();
#else
		return new IO::ConsoleWriter();
#endif
	}
}

Int32 MemCheckError()
{
	Bool found = false;
	UTF8Char buff[256];
	UTF8Char *sptr;
	IO::Writer *console = 0;


	if (mcMemoryCnt)
	{
		sptr = Text::StrInt32(buff, mcMemoryCnt);
		if (console == 0)
			console = MemOpenWriter();
		console->WriteStrC(UTF8STRC("Memory leaks occurs for "));
		console->WriteStrC(buff, (UOSInt)(sptr - buff));
		console->WriteStrC(UTF8STRC(" times at "));
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		sptr = dt.ToString(buff, "yyyy-MM-dd HH:mm:ss");
		console->WriteLineC(buff, (UOSInt)(sptr - buff));
		found = true;
	}

#ifndef _WIN32_WCE
	if (mcHandle)
	{
		void *lastData = 0;
		PROCESS_HEAP_ENTRY ent;
		Bool first = true;
		ent.lpData = 0;
		HANDLE procHand = GetCurrentProcess();
		UInt8 tmpBuff[sizeof(SYMBOL_INFO) + 256];
		SYMBOL_INFO *symb;
		DWORD64 disp;
		Char *cptr;
		IMAGEHLP_LINE64 line;
		UInt32 displacement;
		line.SizeOfStruct = sizeof(line);
		SymInitialize(procHand, 0, TRUE);
		UOSInt i;

		symb = (SYMBOL_INFO*)tmpBuff;
		symb->SizeOfStruct = sizeof(SYMBOL_INFO);
		symb->MaxNameLen = 256;


		while (HeapWalk(mcHandle, &ent))
		{
			if (ent.wFlags & PROCESS_HEAP_ENTRY_BUSY)
			{
				if (first && ent.cbData == 6144)
				{
					first = false;
					continue;
				}
				first = false;
				if (ent.lpData == lastData)
				{
					break;
				}
				sptr = Text::StrConcatC(buff, UTF8STRC("("));
				sptr = Text::StrHexVal32(sptr, (UInt32)(UOSInt)ent.lpData);
				sptr = Text::StrConcatC(sptr, UTF8STRC(") size = "));
				sptr = Text::StrUInt32(sptr, ent.cbData - 8);
				sptr = Text::StrConcatC(sptr, UTF8STRC(": "));
				if (ent.cbData > 24)
				{
					sptr = Text::StrHexBytes(sptr, (UInt8*)ent.lpData + 8, 8, ' ');
					sptr = Text::StrConcatC(sptr, UTF8STRC(".. "));
					sptr = Text::StrHexBytes(sptr, ((UInt8*)ent.lpData) + ent.cbData - 8, 8, ' ');
				}
				else
				{
					sptr = Text::StrHexBytes(sptr, (UInt8*)ent.lpData + 8, ent.cbData - 8, ' ');
				}

				UOSInt address = *(UOSInt*)ent.lpData;
				if (address)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" Alloc from "));
					if (SymFromAddr(procHand, address, &disp, symb))
					{
						cptr = symb->Name;
						while ((*sptr++ = (UTF8Char)*cptr++) != 0);
						sptr--;

						if (SymGetLineFromAddr64(procHand, address, (DWORD*)&displacement, &line))
						{
							sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
							cptr = line.FileName;
							i = Text::StrLastIndexOfChar(cptr, '\\');
							cptr = &cptr[i + 1];
							while ((*sptr++ = (UTF8Char)*cptr++) != 0);
							sptr = Text::StrConcatC(sptr - 1, UTF8STRC("("));
							sptr = Text::StrInt32(sptr, (Int32)line.LineNumber);
							sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
						}

					}
					else
					{
					}
				}

				if (console == 0)
					console = MemOpenWriter();
				console->WriteLineC(buff, (UOSInt)(sptr - buff));
				lastData = ent.lpData;
				found = true;
			}
		}
		SymCleanup(procHand);
	}
#endif
	if (console)
	{
		delete console;
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

void MemIncCounter(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt++;
	Sync::Mutex_Unlock(&mcMut);
}

void MemDecCounter(void *ptr)
{
	Sync::Mutex_Lock(&mcMut);
	mcMemoryCnt--;
	Sync::Mutex_Unlock(&mcMut);
}
