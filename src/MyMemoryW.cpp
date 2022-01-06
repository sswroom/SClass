#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
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
Sync::Mutex *mcMut = 0;
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
	if (Sync::Interlocked::Increment(&mcInitCnt) == 1)
	{
		mcHandle = HeapCreate(0, 0, 0);//GetProcessHeap();
		mcIntHandle = HeapCreate(0, 0, 0);
		mcBusy = 0;
		mcBreakPt = 0;
		mcBreakSize = 0;
		mcMut = new Sync::Mutex();
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

void MemSetLogFile(const UTF8Char *logFile)
{
	if (mcLogFile)
	{
		HeapFree(mcIntHandle, 0, (void*)mcLogFile);
		mcLogFile = 0;
	}
	if (logFile)
	{
		UOSInt size = Text::StrCharCnt(logFile);
		mcLogFile = (const UTF8Char *)HeapAlloc(mcIntHandle, 0, (size + 1) * sizeof(UTF8Char));
		Text::StrConcat((UTF8Char*)mcLogFile, logFile);
	}
}

void MemLock()
{
	mcMut->Lock();
}

void MemUnlock()
{
	mcMut->Unlock();
}

void *MAlloc(UOSInt size)
{
	mcMut->Lock();
	mcMemoryCnt++;

	void *mptr = HeapAlloc(mcHandle, 0, size + 8);
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size + 8))
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

	return ((UInt8*)mptr) + 8;
}

void *MAllocA(UOSInt size)
{
	mcMut->Lock();
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 32);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
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
	mptr += 16 - (15 & (OSInt)mptr);
	*(UInt8**)&mptr[-8] = sptr;
	return mptr;
}

void *MAllocA64(UOSInt size)
{
	mcMut->Lock();
	mcMemoryCnt++;

	UInt8 *mptr = (UInt8*)HeapAlloc(mcHandle, 0, size + 80);
	UInt8 *sptr = mptr;
	if ((OSInt)mptr == mcBreakPt && (mcBreakSize == 0 || mcBreakSize == size))
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
	mcMut->Lock();
	mcMemoryCnt--;

//	_heapchk();
	HeapFree(mcHandle, 0, ((UInt8*)ptr) - 8);
	mcMut->Unlock();
}

void MemFreeA(void *ptr)
{
	mcMut->Lock();
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
	mcMut->Unlock();
}

void MemDeinit()
{
	if (Sync::Interlocked::Decrement(&mcInitCnt) == 0)
	{
		MemCheckError();
		delete mcMut;
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
		Text::StrInt32(buff, mcMemoryCnt);
		if (console == 0)
			console = MemOpenWriter();
		console->WriteStrC(UTF8STRC("Memory leaks occurs for ");
		console->Write(buff);
		console->WriteStrC(UTF8STRC(" times at ");
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		dt.ToString(buff, "yyyy-MM-dd HH:mm:ss");
		console->WriteLine(buff);
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
				sptr = Text::StrConcat(buff, (const UTF8Char*)"(");
				sptr = Text::StrHexVal32(sptr, (UInt32)(UOSInt)ent.lpData);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)") size = ");
				sptr = Text::StrUInt32(sptr, ent.cbData - 8);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)": ");
				if (ent.cbData > 24)
				{
					sptr = Text::StrHexBytes(sptr, (UInt8*)ent.lpData + 8, 8, ' ');
					sptr = Text::StrConcat(sptr, (const UTF8Char*)".. ");
					sptr = Text::StrHexBytes(sptr, ((UInt8*)ent.lpData) + ent.cbData - 8, 8, ' ');
				}
				else
				{
					sptr = Text::StrHexBytes(sptr, (UInt8*)ent.lpData + 8, ent.cbData - 8, ' ');
				}

				UOSInt address = *(UOSInt*)ent.lpData;
				if (address)
				{
					sptr = Text::StrConcat(sptr, (const UTF8Char*)" Alloc from ");
					if (SymFromAddr(procHand, address, &disp, symb))
					{
						cptr = symb->Name;
						while ((*sptr++ = (UTF8Char)*cptr++) != 0);
						sptr--;

						if (SymGetLineFromAddr64(procHand, address, (DWORD*)&displacement, &line))
						{
							sptr = Text::StrConcat(sptr, (const UTF8Char*)" ");
							cptr = line.FileName;
							i = Text::StrLastIndexOf(cptr, '\\');
							cptr = &cptr[i + 1];
							while ((*sptr++ = (UTF8Char)*cptr++) != 0);
							sptr = Text::StrConcat(sptr - 1, (const UTF8Char*)"(");
							sptr = Text::StrInt32(sptr, (Int32)line.LineNumber);
							sptr = Text::StrConcat(sptr, (const UTF8Char*)")");
						}

					}
					else
					{
					}
				}

				if (console == 0)
					console = MemOpenWriter();
				console->WriteLine(buff);
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
	mcMut->Lock();
	mcMemoryCnt++;
	mcMut->Unlock();
}

void MemDecCounter(void *ptr)
{
	mcMut->Lock();
	mcMemoryCnt--;
	mcMut->Unlock();
}
