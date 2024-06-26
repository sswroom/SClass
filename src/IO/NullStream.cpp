#include "Stdafx.h"
#include "IO/NullStream.h"

IO::NullStream::NullStream() : IO::SeekableStream(CSTR("Null"))
{
}

IO::NullStream::~NullStream()
{
}

Bool IO::NullStream::IsDown() const
{
	return false;
}

UOSInt IO::NullStream::Read(const Data::ByteArray &buff)
{
	return 0;
}

UOSInt IO::NullStream::Write(Data::ByteArrayR buff)
{
	return buff.GetSize();
}

Int32 IO::NullStream::Flush()
{
	return 0;
}

void IO::NullStream::Close()
{
}

Bool IO::NullStream::Recover()
{
	return true;
}

IO::StreamType IO::NullStream::GetStreamType() const
{
	return IO::StreamType::Null;
}

UInt64 IO::NullStream::SeekFromBeginning(UInt64 position)
{
	return 0;
}

UInt64 IO::NullStream::SeekFromCurrent(Int64 position)
{
	return 0;
}

UInt64 IO::NullStream::SeekFromEnd(Int64 position)
{
	return 0;
}

UInt64 IO::NullStream::GetPosition()
{
	return 0;
}

UInt64 IO::NullStream::GetLength()
{
	return 0;
}
