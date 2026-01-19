#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileViewStream.h"

IO::FileViewStream::FileViewStream(Text::CStringNN fileName) : IO::SeekableStream(fileName)
{
	NEW_CLASS(this->vfb, IO::ViewFileBuffer(fileName.v));
	this->length = this->vfb->GetLength();
	this->currPos = 0;
	this->fptr = this->vfb->GetPointer();
}

IO::FileViewStream::~FileViewStream()
{
	Close();
}

Bool IO::FileViewStream::IsDown() const
{
	return this->vfb == 0 || this->fptr.IsNull();
}

UIntOS IO::FileViewStream::Read(const Data::ByteArray &buff)
{
	UnsafeArray<UInt8> fptr;
	if (!this->fptr.SetTo(fptr))
		return 0;
	UInt64 endPtr = this->currPos + buff.GetSize();
	if (endPtr > this->length)
		endPtr = this->length;
	if (endPtr > this->currPos)
	{
		MemCopyNO(buff.Ptr(), &fptr[this->currPos], (UIntOS)(endPtr - this->currPos));
		UIntOS size = (UIntOS)(endPtr - this->currPos);
		this->currPos = endPtr;
		return size;
	}
	else
	{
		return 0;
	}
}

UIntOS IO::FileViewStream::Write(Data::ByteArrayR buff)
{
	return 0;
}

Int32 IO::FileViewStream::Flush()
{
	return 0;
}

void IO::FileViewStream::Close()
{
	this->fptr = nullptr;
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

IO::StreamType IO::FileViewStream::GetStreamType() const
{
	return IO::StreamType::FileView;
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
	if (this->vfb == 0 || this->fptr.IsNull())
		return true;
	return false;
}
