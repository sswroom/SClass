#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/MemoryStream.h"
#include "Text/MyString.h"
#include <stdio.h>

#if defined(CPU_AVR)
#define MAX_CAPACITY 1024
#define DEF_CAPACITY 128
#else
#define MAX_CAPACITY 1048576000
#define DEF_CAPACITY 1024
#endif

IO::MemoryStream::MemoryStream(const UTF8Char *dbg) : IO::SeekableStream((const UTF8Char*)"MemoryStream")
{
	capacity = DEF_CAPACITY;
	currSize = 0;
	currPtr = 0;
	memPtr = MemAlloc(UInt8, capacity);
	this->dbg = Text::StrCopyNew(dbg);
}

IO::MemoryStream::MemoryStream(UOSInt capacity, const UTF8Char *dbg) : IO::SeekableStream((const UTF8Char*)"MemoryStream")
{
	if (capacity == 0)
	{
		capacity = DEF_CAPACITY;
	}
	this->capacity = capacity;
	currSize = 0;
	currPtr = 0;
	memPtr = MemAlloc(UInt8, this->capacity);
	this->dbg = Text::StrCopyNew(dbg);
}

IO::MemoryStream::MemoryStream(UInt8 *buff, UOSInt buffSize, const UTF8Char *dbg) : IO::SeekableStream((const UTF8Char*)"MemoryStream")
{
	this->capacity = 0;
	this->currSize = buffSize;
	this->currPtr = 0;
	this->memPtr = buff;
	this->dbg = Text::StrCopyNew(dbg);
}

IO::MemoryStream::~MemoryStream()
{
	if (capacity != 0)
	{
		MemFree(memPtr);
	}
	Text::StrDelNew(this->dbg);
}

UInt8 *IO::MemoryStream::GetBuff(UOSInt *buffSize)
{
	*buffSize = currSize;
	return this->memPtr;
}

UOSInt IO::MemoryStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt readSize = size;
	if (this->currSize - this->currPtr < readSize)
	{
		readSize = this->currSize - this->currPtr;
	}
	MemCopyNO(buff, &this->memPtr[this->currPtr], readSize);
	this->currPtr += readSize;
	return readSize;
}

UOSInt IO::MemoryStream::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt endPos = this->currPtr + size;

	if (endPos <= this->capacity)
	{
		MemCopyNO(&this->memPtr[this->currPtr], buff, size);
		if (endPos > this->currSize)
			this->currSize = endPos;
		this->currPtr = endPos;
		return size;
	}
	if (endPos > MAX_CAPACITY)
	{
		size = MAX_CAPACITY - this->currPtr;
		endPos = MAX_CAPACITY;
	}
	while (endPos > this->capacity)
	{
		UOSInt newCapacity = this->capacity << 1;
		UInt8 *newPtr;
		if (newCapacity > MAX_CAPACITY)
		{
			newCapacity = MAX_CAPACITY;
		}
		newPtr = MemAlloc(UInt8, newCapacity);
		MemCopyNO(newPtr, this->memPtr, this->currSize);
		MemFree(this->memPtr);
		this->memPtr = newPtr;
		this->capacity = newCapacity;
	}

	MemCopyNO(&this->memPtr[this->currPtr], buff, size);
	if (endPos > this->currSize)
		this->currSize = endPos;
	this->currPtr = endPos;
	return size;
}

Int32 IO::MemoryStream::Flush()
{
	return 0;
}

void IO::MemoryStream::Close()
{
}

Bool IO::MemoryStream::Recover()
{
	return true;
}

UInt64 IO::MemoryStream::SeekFromBeginning(UInt64 position)
{
	UInt64 outPos = position;

	if (this->capacity == 0)
	{
		if (outPos > currSize)
			return this->currPtr;
		this->currPtr = (UOSInt)outPos;
		return this->currPtr;
	}

	if (outPos > MAX_CAPACITY)
	{
		return this->currPtr;
	}
	else if (outPos > currSize)
	{
		return this->currPtr;
	}

	while (outPos > this->capacity)
	{
		UInt8 *outPtr = MemAlloc(UInt8, this->capacity << 1);
		MemCopyNO(outPtr, this->memPtr, this->currSize);
		this->capacity = this->capacity << 1;
		MemFree(this->memPtr);
		this->memPtr = outPtr;
	}

	if (this->currSize < outPos)
	{
		this->currSize = (UOSInt)outPos;
	}

	this->currPtr = (UOSInt)outPos;
	return outPos;
}


UInt64 IO::MemoryStream::SeekFromCurrent(Int64 position)
{
	UInt64 outPos;
	if (position < 0 && this->currPtr < (UInt64)-position)
		outPos = 0;
	else
		outPos = (UInt64)((Int64)this->currPtr + position);
	return SeekFromBeginning(outPos);
}


UInt64 IO::MemoryStream::SeekFromEnd(Int64 position)
{
	UInt64 outPos;
	if (position < 0 && this->currSize < (UInt64)-position)
		outPos = 0;
	else
		outPos = (UInt64)((Int64)this->currSize + position);
	return SeekFromBeginning(outPos);
}

UInt64 IO::MemoryStream::GetPosition()
{
	return this->currPtr;
}

UInt64 IO::MemoryStream::GetLength()
{
	return this->currSize;
}

void IO::MemoryStream::Clear()
{
	this->currPtr = 0;
	this->currSize = 0;
}
