#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Stream.h"
#include "IO/MemoryStream.h"
#include "Sync/Event.h"

#if defined(CPU_AVR)
#define MAX_CAPACITY 1024
#define DEF_CAPACITY 128
#else
#define MAX_CAPACITY 1048576000
#define DEF_CAPACITY 1024
#endif

IO::MemoryStream::MemoryStream() : IO::SeekableStream(CSTR("MemoryStream")), buff(DEF_CAPACITY)
{
	currSize = 0;
	currPtr = 0;
}

IO::MemoryStream::MemoryStream(UOSInt capacity) : IO::SeekableStream(CSTR("MemoryStream")), buff((capacity == 0)?DEF_CAPACITY:capacity)
{
	currSize = 0;
	currPtr = 0;
}

/*IO::MemoryStream::MemoryStream(UInt8 *buff, UOSInt buffSize, const UTF8Char *dbg, UOSInt dbgLen) : IO::SeekableStream(CSTR("MemoryStream"))
{
	this->capacity = 0;
	this->currSize = buffSize;
	this->currPtr = 0;
	this->memPtr = buff;
}*/

IO::MemoryStream::~MemoryStream()
{
}

UnsafeArray<UInt8> IO::MemoryStream::GetBuff()
{
	return this->buff.Arr();
}

UnsafeArray<UInt8> IO::MemoryStream::GetBuff(OutParam<UOSInt> buffSize)
{
	buffSize.Set(this->currSize);
	return this->buff.Arr();
}

Data::ByteArray IO::MemoryStream::GetArray() const
{
	return this->buff.SubArray(0, currSize);
}

Text::CStringNN IO::MemoryStream::ToCString()
{
	if (this->currSize < this->buff.GetSize())
	{
		this->buff[this->currSize] = 0;
	}
	else
	{
		UOSInt newCapacity = this->buff.GetSize() << 1;
		if (newCapacity > MAX_CAPACITY)
		{
			newCapacity = MAX_CAPACITY;
		}
		Data::ByteBuffer newBuff(newCapacity);
		newBuff.CopyFrom(0, this->buff.SubArray(0, this->currSize));
		this->buff.ReplaceBy(newBuff);
		this->buff[this->currSize] = 0;
	}
	return Text::CStringNN(this->buff.Arr(), this->currSize);
}

Bool IO::MemoryStream::IsDown() const
{
	return false;
}

UOSInt IO::MemoryStream::Read(const Data::ByteArray &buff)
{
	UOSInt readSize = buff.GetSize();
	if (this->currSize - this->currPtr < readSize)
	{
		readSize = this->currSize - this->currPtr;
	}
	buff.CopyFrom(0, this->buff.SubArray(this->currPtr, readSize));
	this->currPtr += readSize;
	return readSize;
}

UOSInt IO::MemoryStream::Write(Data::ByteArrayR buff)
{
	UOSInt endPos = this->currPtr + buff.GetSize();

	if (endPos <= this->buff.GetSize())
	{
		this->buff.CopyFrom(this->currPtr, buff);
		if (endPos > this->currSize)
			this->currSize = endPos;
		this->currPtr = endPos;
		return buff.GetSize();
	}
	if (endPos > MAX_CAPACITY)
	{
		buff = buff.WithSize(MAX_CAPACITY - this->currPtr);
		endPos = MAX_CAPACITY;
	}
	while (endPos > this->buff.GetSize())
	{
		UOSInt newCapacity = this->buff.GetSize() << 1;
		if (newCapacity > MAX_CAPACITY)
		{
			newCapacity = MAX_CAPACITY;
		}
		Data::ByteBuffer newBuff(newCapacity);
		newBuff.CopyFrom(0, this->buff.SubArray(0, this->currSize));
		this->buff.ReplaceBy(newBuff);
	}

	this->buff.CopyFrom(this->currPtr, buff);
	if (endPos > this->currSize)
		this->currSize = endPos;
	this->currPtr = endPos;
	return buff.GetSize();
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

IO::StreamType IO::MemoryStream::GetStreamType() const
{
	return IO::StreamType::Memory;
}

UInt64 IO::MemoryStream::SeekFromBeginning(UInt64 position)
{
	UInt64 outPos = position;
	if (outPos > MAX_CAPACITY)
	{
		return this->currPtr;
	}
	else if (outPos > currSize)
	{
		return this->currPtr;
	}

	while (outPos > this->buff.GetSize())
	{
		Data::ByteBuffer tmpBuff(this->buff.GetSize() << 1);
		tmpBuff.CopyFrom(0, this->buff.SubArray(0, this->currSize));
		this->buff.ReplaceBy(tmpBuff);
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
