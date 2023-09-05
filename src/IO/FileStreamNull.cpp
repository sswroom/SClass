#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"

IO::FileStream::FileStream(NotNullPtr<Text::String> fileName, IO::FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	this->handle = 0;
	this->currPos = -1;
	this->handle = 0;
	return;
}

IO::FileStream::FileStream(Text::CStringNN fileName, FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	this->handle = 0;
	this->currPos = -1;
	this->handle = 0;
}

IO::FileStream::~FileStream()
{
}

Bool IO::FileStream::IsDown() const
{
	return true;
}

Bool IO::FileStream::IsError() const
{
	return true;
}

UOSInt IO::FileStream::Read(const Data::ByteArray &buff)
{
	return 0;
}

UOSInt IO::FileStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 IO::FileStream::Flush()
{
	return 0;
}

void IO::FileStream::Close()
{
}

Bool IO::FileStream::Recover()
{
	return false;
}

UInt64 IO::FileStream::SeekFromBeginning(UInt64 position)
{
	return 0;
}

UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	return 0;
}

UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	return 0;
}

UInt64 IO::FileStream::GetPosition()
{
	return this->currPos;
}

UInt64 IO::FileStream::GetLength()
{
	return 0;
}

void IO::FileStream::SetLength(UInt64 newLength)
{
}

Int32 IO::FileStream::GetErrCode()
{
}

void IO::FileStream::GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
}

void IO::FileStream::SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
}

UOSInt IO::FileStream::LoadFile(Text::CStringNN fileName, UInt8 *buff, UOSInt maxBuffSize)
{
	IO::FileStream fs(fileName, FileMode::ReadOnly, FileShare::DenyNone, BufferType::Normal);
	if (fs.IsError())
	{
		return 0;
	}
	UInt64 fileLen = fs.GetLength();
	if (fileLen > maxBuffSize || fileLen == 0)
	{
		return 0;
	}
	UOSInt readSize = fs.Read(Data::ByteArray(buff, maxBuffSize));
	if (readSize == fileLen)
	{
		return readSize;
	}
	return 0;
}
