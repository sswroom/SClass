#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ViewFileBuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>


IO::ViewFileBuffer::ViewFileBuffer(const UTF8Char *fileName)
{
	this->filePtr = 0;

	this->fileHandle = (void*)(OSInt)open((const Char*)fileName, O_RDWR);
	if ((OSInt)this->fileHandle < 0)
	{
		return;
	}
	this->mapHandle = mmap(0, (size_t)GetLength(), PROT_READ|PROT_WRITE, MAP_PRIVATE, (int)(OSInt)this->fileHandle, 0);
	if ((OSInt)this->mapHandle < 0)
	{
		return;
	}
}

IO::ViewFileBuffer::~ViewFileBuffer()
{
	if ((OSInt)this->fileHandle >= 0)
	{
		if ((OSInt)this->mapHandle >= 0)
		{
			munmap(mapHandle, (size_t)GetLength());
		}
		close((int)(OSInt)fileHandle);
	}
}

UnsafeArrayOpt<UInt8> IO::ViewFileBuffer::GetPointer()
{
	if ((OSInt)this->fileHandle < 0 || (OSInt)this->mapHandle < 0)
	{
		return 0;
	}
	return (UInt8*)mapHandle;
}

UInt64 IO::ViewFileBuffer::GetLength()
{
	UInt64 pos = (UInt64)lseek((int)(OSInt)fileHandle, 0, SEEK_CUR);
	UInt64 leng = (UInt64)lseek((int)(OSInt)fileHandle, 0, SEEK_END);
	lseek((int)(OSInt)fileHandle, (off_t)pos, SEEK_SET);
	return leng;
}

void IO::ViewFileBuffer::FreePointer()
{
}

