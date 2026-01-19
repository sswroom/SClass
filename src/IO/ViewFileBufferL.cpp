#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ViewFileBuffer.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>


IO::ViewFileBuffer::ViewFileBuffer(UnsafeArray<const UTF8Char> fileName)
{
	this->filePtr = nullptr;

	this->fileHandle = (void*)(IntOS)open((const Char*)fileName.Ptr(), O_RDWR);
	if ((IntOS)this->fileHandle < 0)
	{
		return;
	}
	this->mapHandle = mmap(0, (size_t)GetLength(), PROT_READ|PROT_WRITE, MAP_PRIVATE, (int)(IntOS)this->fileHandle, 0);
	if ((IntOS)this->mapHandle < 0)
	{
		return;
	}
}

IO::ViewFileBuffer::~ViewFileBuffer()
{
	if ((IntOS)this->fileHandle >= 0)
	{
		if ((IntOS)this->mapHandle >= 0)
		{
			munmap(mapHandle, (size_t)GetLength());
		}
		close((int)(IntOS)fileHandle);
	}
}

UnsafeArrayOpt<UInt8> IO::ViewFileBuffer::GetPointer()
{
	if ((IntOS)this->fileHandle < 0 || (IntOS)this->mapHandle < 0)
	{
		return nullptr;
	}
	return (UInt8*)mapHandle;
}

UInt64 IO::ViewFileBuffer::GetLength()
{
	UInt64 pos = (UInt64)lseek((int)(IntOS)fileHandle, 0, SEEK_CUR);
	UInt64 leng = (UInt64)lseek((int)(IntOS)fileHandle, 0, SEEK_END);
	lseek((int)(IntOS)fileHandle, (off_t)pos, SEEK_SET);
	return leng;
}

void IO::ViewFileBuffer::FreePointer()
{
}

