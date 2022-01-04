#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileViewStream.h"

IO::FileViewStream::FileViewStream(const UTF8Char *fileName) : IO::SeekableStream(fileName)
{
	NEW_CLASS(this->vfb, IO::ViewFileBuffer(fileName));
	this->length = this->vfb->GetLength();
	this->currPos = 0;
	this->fptr = this->vfb->GetPointer();
}

IO::FileViewStream::~FileViewStream()
{
	Close();
}

UOSInt IO::FileViewStream::Read(UInt8 *buff, UOSInt size)
{
	if (this->fptr == 0)
		return 0;
	UInt64 endPtr = this->currPos + size;
	if (endPtr > this->length)
		endPtr = this->length;
	if (endPtr > this->currPos)
	{
		MemCopyNO(buff, &this->fptr[this->currPos], (UOSInt)(endPtr - this->currPos));
		UOSInt size = (UOSInt)(endPtr - this->currPos);
		this->currPos = endPtr;
		return size;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::FileViewStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 IO::FileViewStream::Flush()
{
	return 0;
}

void IO::FileViewStream::Close()
{
	this->fptr = 0;
	if (this->vfb)
	{
		DEL_CLASS(this->vfb);
		this->vfb = 0;
	}
}

Bool IO::FileViewStream::Recover()
{
	////////////////////////////////////
	return false;
}

UInt64 IO::FileViewStream::SeekFromBeginning(UInt64 position)
{
	this->currPos = position;
	if (this->currPos > this->length)
		this->currPos = this->length;
	else if (this->currPos < 0)
		this->currPos = 0;
	return this->currPos;
}

UInt64 IO::FileViewStream::SeekFromCurrent(Int64 position)
{
	this->currPos += position;
	if (this->currPos > this->length)
		this->currPos = this->length;
	else if (this->currPos < 0)
		this->currPos = 0;
	return this->currPos;
}

UInt64 IO::FileViewStream::SeekFromEnd(Int64 position)
{
	this->currPos = this->length + position;
	if (this->currPos > this->length)
		this->currPos = this->length;
	else if (this->currPos < 0)
		this->currPos = 0;
	return this->currPos;
}

UInt64 IO::FileViewStream::GetPosition()
{
	return this->currPos;	
}

UInt64 IO::FileViewStream::GetLength()
{
	return this->length;
}

Bool IO::FileViewStream::IsError()
{
	if (this->vfb == 0 || this->fptr == 0)
		return true;
	return false;
}
