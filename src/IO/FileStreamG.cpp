#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include <stdio.h>
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#define SUPPORT_SHARE
#include <io.h>
#include <share.h>
#endif

IO::FileStream::FileStream(NN<Text::String> fileName, FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	handle = 0;
#ifndef SUPPORT_SHARE
	if (mode == IO::FileMode::Create)
	{
		handle = fopen((const char*)fileName->v, "wb+");
		currPos = 0;
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = fopen((const char*)fileName->v, "wb");
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = fopen((const char*)fileName->v, "ab+");
		if (handle)
		{
			this->currPos = ftell((FILE*)handle);
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = fopen((const char*)fileName->v, "rb");
		currPos = 0;
	}
#else
	Int32 shflag;
	if (share == IO::FileShare::DenyNone)
	{
		shflag = _SH_DENYNO;
	}
	else if (share == IO::FileShare::DenyRead)
	{
		shflag = _SH_DENYRD;
	}
	else if (share == IO::FileShare::DenyWrite)
	{
		shflag = _SH_DENYWR;
	}
	else
	{
		shflag = _SH_DENYRW;
	}

	if (mode == IO::FileMode::Create)
	{
		handle = _fsopen((const char*)fileName->v, "wb+", shflag);
		currPos = 0;
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = _fsopen((const char*)fileName->v, "wb", shflag);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = _fsopen((const char*)fileName->v, "ab+", shflag);
		if (handle)
		{
#if _MSC_VER >= 1400
			this->currPos = _ftelli64((FILE*)handle);
#else
			this->currPos = ftell((FILE*)handle);
#endif
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = _fsopen((const char*)fileName->v, "rb", shflag);
		currPos = 0;
	}
#endif
}

IO::FileStream::~FileStream()
{
	Close();
}

Bool IO::FileStream::IsDown() const
{
	return this->handle == 0;
}

Bool IO::FileStream::IsError() const
{
	return this->handle == 0;
}

UOSInt IO::FileStream::Read(const Data::ByteArray &buff)
{
	if (handle == 0)
		return 0;
	OSInt readSize = fread(buff.Ptr(), 1, buff.GetSize(), (FILE*)handle);
	this->currPos += (UOSInt)readSize;
	return (UOSInt)readSize;
}

UOSInt IO::FileStream::Write(const UInt8 *buff, UOSInt size)
{
	if (handle == 0)
		return 0;
	OSInt readSize = fwrite(buff, 1, size, (FILE*)handle);
	this->currPos += (UOSInt)readSize;
	return (UOSInt)readSize;
}

Int32 IO::FileStream::Flush()
{
	if (handle == 0)
		return 0;
	return fflush((FILE*)handle);
}

void IO::FileStream::Close()
{
	if (handle)
	{
		fclose((FILE*)handle);
		handle = 0;
	}
}

Bool IO::FileStream::Recover()
{
	////////////////////////////////////////
	return false;
}

UInt64 IO::FileStream::SeekFromBeginning(UInt64 position)
{
	if (handle == 0)
		return 0;
#if _MSC_VER >= 1400 && defined(SUPPORT_SHARE)
	_fseeki64((FILE*)handle, position, SEEK_SET);
	this->currPos = _ftelli64((FILE*)handle);
	return this->currPos;
#else
	fseek((FILE*)handle, (Int32)position, SEEK_SET);
	this->currPos = ftell((FILE*)handle);
	return this->currPos;
#endif
	return 0;
}

UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	if (handle == 0)
		return 0;
#if _MSC_VER >= 1400 && defined(SUPPORT_SHARE)
	_fseeki64((FILE*)handle, position, SEEK_CUR);
	this->currPos = (UInt64)_ftelli64((FILE*)handle);
	return this->currPos;
#else
	fseek((FILE*)handle, (Int32)position, SEEK_CUR);
	this->currPos = (UInt64)ftell((FILE*)handle);
	return this->currPos;
#endif
	return 0;
}

UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	if (handle == 0)
		return -1;
#if _MSC_VER >= 1400 && defined(SUPPORT_SHARE)
	_fseeki64((FILE*)handle, position, SEEK_END);
	this->currPos = (UInt64)_ftelli64((FILE*)handle);
	return this->currPos;
#else
	fseek((FILE*)handle, (Int32)position, SEEK_END);
	this->currPos = (UInt64)ftell((FILE*)handle);
	return this->currPos;
#endif
	return 0;
}

UInt64 IO::FileStream::GetPosition()
{
	return this->currPos;
}

#ifdef _MSC_VER
#ifndef SUPPORT_SHARE
UInt64 IO::FileStream::GetLength()
{
	Int32 hand = (Int32)_fileno((FILE*)this->handle);
	return _filelengthi64(hand);
}
#else
UInt64 IO::FileStream::GetLength()
{
	Int32 hand = (Int32)_fileno((FILE*)this->handle);
	return _filelengthi64(hand);
}
#endif
#else
UInt64 IO::FileStream::GetLength()
{
	UInt64 pos = this->currPos;
	UInt64 leng = SeekFromEnd(0);
	SeekFromBeginning(pos);
	return leng;
}
#endif

void IO::FileStream::SetLength(UInt64 newLength)
{
}

Int32 IO::FileStream::GetErrCode()
{
	if (handle)
	{
		return ferror((FILE*)handle);
	}
	else
	{
		return -1;
	}
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
