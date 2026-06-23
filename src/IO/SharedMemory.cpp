#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SharedMemory.h"
#include "Text/MyStringW.h"
#include <windows.h>

IO::SharedMemory::SharedMemory(void *hand, UInt8 *memPtr, IntOS memSize)
{
	this->hand = hand;
	this->memPtr = memPtr;
	this->memSize = memSize;
}

IO::SharedMemory::~SharedMemory()
{
	UnmapViewOfFile(this->memPtr);
	CloseHandle(this->hand);
}

UInt8 *IO::SharedMemory::GetPointer()
{
	return this->memPtr;
}

Optional<IO::SharedMemory> IO::SharedMemory::Create(UnsafeArray<const UTF8Char> name, IntOS size)
{
	UnsafeArray<const WChar> wname = Text::StrToWCharNew(name);
	HANDLE hMapFile;
	UInt8 *memPtr;
	hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (UInt32)size, wname.Ptr());
	Text::StrDelNew(wname);
	if (hMapFile == 0)
	{
		return nullptr;
	}
	memPtr = (UInt8*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (memPtr == 0)
	{
		CloseHandle(hMapFile);
		return nullptr;
	}
	NN<IO::SharedMemory> smem;
	NEW_CLASSNN(smem, IO::SharedMemory(hMapFile, memPtr, size));
	return smem;
}

Optional<IO::SharedMemory> IO::SharedMemory::Open(UnsafeArray<const UTF8Char> name, IntOS size)
{
	HANDLE hMapFile;
	UInt8 *memPtr;
	UnsafeArray<const WChar> wname = Text::StrToWCharNew(name);
	hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, wname.Ptr());
	Text::StrDelNew(wname);

	if (hMapFile == 0)
	{
		return nullptr;
	}
	
	memPtr = (UInt8*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (memPtr == 0)
	{
		CloseHandle(hMapFile);
		return nullptr;
	}
	NN<IO::SharedMemory> smem;
	NEW_CLASSNN(smem, IO::SharedMemory(hMapFile, memPtr, size));
	return smem;
}
