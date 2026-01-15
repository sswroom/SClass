#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryReadingStream.h"

IO::MemoryReadingStream::MemoryReadingStream(UnsafeArray<const UTF8Char> buff, UIntOS buffSize) : IO::SeekableStream(CSTR("MemoryReadingStream")), buff(buff, buffSize)
{
	this->currPtr = 0;
}

IO::MemoryReadingStream::MemoryReadingStream(Data::ByteArrayR buff) : IO::SeekableStream(CSTR("MemoryReadingStream")), buff(buff)
{
	this->currPtr = 0;
}

IO::MemoryReadingStream::~MemoryReadingStream()
{
}

Bool IO::MemoryReadingStream::IsDown() const
{
	return false;
}

UIntOS IO::MemoryReadingStream::Read(const Data::ByteArray &buff)
{
	UIntOS readSize = buff.GetSize();
	if (this->buff.GetSize() - this->currPtr < readSize)
	{
		readSize = this->buff.GetSize() - this->currPtr;
	}
	buff.CopyFrom(0, this->buff.SubArray(this->currPtr, readSize));
	this->currPtr += readSize;
	return readSize;
}

UIntOS IO::MemoryReadingStream::Write(Data::ByteArrayR buff)
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

	if (outPos > this->buff.GetSize())
		return this->currPtr;
	this->currPtr = (UIntOS)outPos;
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
	if (position < 0 && this->buff.GetSize() < (UInt64)-position)
		outPos = 0;
	else
		outPos = (UInt64)((Int64)this->buff.GetSize() + position);
	return SeekFromBeginning(outPos);
}

UInt64 IO::MemoryReadingStream::GetPosition()
{
	return this->currPtr;
}

UInt64 IO::MemoryReadingStream::GetLength()
{
	return this->buff.GetSize();
}
