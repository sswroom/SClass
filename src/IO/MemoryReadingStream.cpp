#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryReadingStream.h"

IO::MemoryReadingStream::MemoryReadingStream(const UTF8Char *buff, UOSInt buffSize) : IO::SeekableStream(CSTR("MemoryReadingStream"))
{
	this->buff = buff;
	this->buffSize = buffSize;
	this->currPtr = 0;
}

IO::MemoryReadingStream::~MemoryReadingStream()
{
}

Bool IO::MemoryReadingStream::IsDown() const
{
	return false;
}

UOSInt IO::MemoryReadingStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt readSize = size;
	if (this->buffSize - this->currPtr < readSize)
	{
		readSize = this->buffSize - this->currPtr;
	}
	MemCopyNO(buff, &this->buff[this->currPtr], readSize);
	this->currPtr += readSize;
	return readSize;
}

UOSInt IO::MemoryReadingStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 IO::MemoryReadingStream::Flush()
{
	return 0;
}

void IO::MemoryReadingStream::Close()
{
}

Bool IO::MemoryReadingStream::Recover()
{
	return true;
}

IO::StreamType IO::MemoryReadingStream::GetStreamType() const
{
	return IO::StreamType::MemoryReading;
}

UInt64 IO::MemoryReadingStream::SeekFromBeginning(UInt64 position)
{
	UInt64 outPos = position;

	if (outPos > this->buffSize)
		return this->currPtr;
	this->currPtr = (UOSInt)outPos;
	return this->currPtr;
}


UInt64 IO::MemoryReadingStream::SeekFromCurrent(Int64 position)
{
	UInt64 outPos;
	if (position < 0 && this->currPtr < (UInt64)-position)
		outPos = 0;
	else
		outPos = (UInt64)((Int64)this->currPtr + position);
	return SeekFromBeginning(outPos);
}


UInt64 IO::MemoryReadingStream::SeekFromEnd(Int64 position)
{
	UInt64 outPos;
	if (position < 0 && this->buffSize < (UInt64)-position)
		outPos = 0;
	else
		outPos = (UInt64)((Int64)this->buffSize + position);
	return SeekFromBeginning(outPos);
}

UInt64 IO::MemoryReadingStream::GetPosition()
{
	return this->currPtr;
}

UInt64 IO::MemoryReadingStream::GetLength()
{
	return this->buffSize;
}
