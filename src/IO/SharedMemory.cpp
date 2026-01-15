#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SharedMemory.h"
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

IO::SharedMemory *IO::SharedMemory::Create(const WChar *name, IntOS size)
{
	HANDLE hMapFile;
	UInt8 *memPtr;
	hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (UInt32)size, name);
	if (hMapFile == 0)
	{
		return 0;
	}
	memPtr = (UInt8*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (memPtr == 0)
	{
		CloseHandle(hMapFile);
		return 0;
	}
	IO::SharedMemory *smem;
	NEW_CLASS(smem, IO::SharedMemory(hMapFile, memPtr, size));
	return smem;
}

IO::SharedMemory *IO::SharedMemory::Open(const WChar *name, IntOS size)
{
	HANDLE hMapFile;
	UInt8 *memPtr;
	
	hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name);

	if (hMapFile == 0)
	{
		return 0;
	}
	
	memPtr = (UInt8*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (memPtr == 0)
	{
		CloseHandle(hMapFile);
		return 0;
	}
	IO::SharedMemory *smem;
	NEW_CLASS(smem, IO::SharedMemory(hMapFile, memPtr, size));
	return smem;
}
