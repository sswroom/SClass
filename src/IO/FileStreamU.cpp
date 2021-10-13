#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "Sync/Event.h"
#include "Text/StringBuilder.h"
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>

typedef struct
{
	const UTF8Char *fileName;
	Int32 hand;
} ClassData;

IO::FileStream::FileStream(const UTF8Char *fileName, FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	this->handle = 0;
	if (fileName == 0)
	{
		this->currPos = 0;
		this->handle = 0;
		return;
	}
	else if (*fileName == 0)
	{
		this->currPos = 0;
		this->handle = 0;
		return;
	}

	int flags = 0;
#if defined(O_BINARY)
	flags |= O_BINARY;
#endif
	mode_t opmode = S_IRUSR | S_IWUSR;
	ClassData *clsData = MemAlloc(ClassData, 1);
	this->handle = clsData;
	clsData->fileName = Text::StrCopyNew(fileName);

	if (mode == FileMode::Create)
	{
		flags |= O_CREAT|O_RDWR|O_TRUNC;
	}
	else if (mode == FileMode::CreateWrite)
	{
		flags |= O_CREAT|O_WRONLY;
	}
	else if (mode == FileMode::Append)
	{
		flags |= O_CREAT|O_RDWR;
	}
	else if (mode == FileMode::ReadOnly)
	{
		flags |= O_RDONLY;
	}
	else if (mode == FileMode::ReadWriteExisting)
	{
		flags |= O_RDWR;
	}
	else if (mode == FileMode::Device)
	{
		flags |= O_RDWR;
	}

	switch (buffType)
	{
	case BufferType::NoWriteBuffer:
#if defined(__FreeBSD__)
		flags |= O_SYNC;
#else
		flags |= O_DSYNC;
#endif
		break;
	case BufferType::RandomAccess:
	case BufferType::Normal:
	case BufferType::Sequential:
	case BufferType::NoBuffer:
	default:
		break;
	}

#if defined(__USE_LARGEFILE64)
	clsData->hand = open64((const Char*)fileName, flags, opmode);
#else
	clsData->hand = open((const Char*)fileName, flags, opmode);
#endif
	if (clsData->hand == -1)
	{
		clsData->hand = 0;
	}
	if (mode == FileMode::Append && clsData->hand > 0)
	{
		this->SeekFromEnd(0);
	}
	else
	{
		this->currPos = 0;
	}
}

IO::FileStream::~FileStream()
{
	Close();
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData)
	{
		Text::StrDelNew(clsData->fileName);
		MemFree(clsData);
	}
}

Bool IO::FileStream::IsError()
{
	if (this->handle == 0)
		return true;
	ClassData *clsData = (ClassData*)this->handle;
	return clsData->hand == 0;
}

UOSInt IO::FileStream::Read(UInt8 *buff, UOSInt size)
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
	OSInt readSize = read(clsData->hand, buff, size);
	if (readSize >= 0)
	{
		this->currPos += (UOSInt)readSize;
		return (UOSInt)readSize;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::FileStream::Write(const UInt8 *buff, UOSInt size)
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
	OSInt readSize = write(clsData->hand, buff, size);
	if (readSize >= 0)
	{
		this->currPos += (UOSInt)readSize;
		return (UOSInt)readSize;
	}
	else
	{
		return 0;
	}
}

Int32 IO::FileStream::Flush()
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
	return fsync(clsData->hand);
}

void IO::FileStream::Close()
{
	if (this->handle)
	{
		ClassData *clsData = (ClassData*)this->handle;
		if (clsData->hand)
		{
			close(clsData->hand);
			clsData->hand = 0;
		}
	}
}

Bool IO::FileStream::Recover()
{
	return false;
}

UInt64 IO::FileStream::SeekFromBeginning(UInt64 position)
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek(clsData->hand, (Int64)position, SEEK_SET);
#else
	this->currPos = (UInt64)lseek64(clsData->hand, (Int64)position, SEEK_SET);
#endif
	return this->currPos;
}


UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek(clsData->hand, position, SEEK_CUR);
#else
	this->currPos = (UInt64)lseek64(clsData->hand, position, SEEK_CUR);
#endif
	return this->currPos;
}


UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	if (this->handle == 0)
		return 0;
	ClassData *clsData = (ClassData*)this->handle;
	if (clsData->hand == 0)
		return 0;
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek(clsData->hand, position, SEEK_END);
#else
	this->currPos = (UInt64)lseek64(clsData->hand, position, SEEK_END);
#endif
	return this->currPos;
}

UInt64 IO::FileStream::GetPosition()
{
	return currPos;
}

UInt64 IO::FileStream::GetLength()
{
	UInt64 pos = this->currPos;
	UInt64 leng = this->SeekFromEnd(0);
	this->SeekFromBeginning(pos);
	return leng;
}

void IO::FileStream::SetLength(UInt64 newLength)
{
	if (this->handle == 0)
		return;
#if defined(__APPLE__)
	ClassData *clsData = (ClassData*)this->handle;
	fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, newLength};
	int ret = fcntl((int)clsData->hand, F_PREALLOCATE, &store);
	if (ret == -1)
	{
		store.fst_flags = F_ALLOCATEALL;
		fcntl((int)clsData->hand, F_PREALLOCATE, &store);
	}
#elif !defined(__ANDROID__)
	ClassData *clsData = (ClassData*)this->handle;
	posix_fallocate(clsData->hand, (off_t)this->currPos, (off_t)(newLength - this->currPos));
#endif
}

Int32 IO::FileStream::GetErrCode()
{
	if (handle)
	{
		return errno;
	}
	else
	{
		return -1;
	}
}

void IO::FileStream::GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
	if (handle == 0)
		return;
	ClassData *clsData = (ClassData*)this->handle;
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	if (clsData->hand == 0)
	{
		if (stat64((const Char*)clsData->fileName, &s) != 0)
			return;
	}
	else
	{
		if (fstat64(clsData->hand, &s) != 0)
			return;
	}
#else
	struct stat s;
	if (clsData->hand == 0)
	{
		if (stat((const Char*)clsData->fileName, &s) != 0)
			return;
	}
	else
	{
		if (fstat(clsData->hand, &s) != 0)
			return;
	}
#endif
#if defined(__APPLE__)
	if (creationTime)
	{
		creationTime->SetUnixTimestamp(s.st_ctimespec.tv_sec);
	}
	if (lastAccessTime)
	{
		lastAccessTime->SetUnixTimestamp(s.st_atimespec.tv_sec);
	}
	if (lastWriteTime)
	{
		lastWriteTime->SetUnixTimestamp(s.st_mtimespec.tv_sec);
	}
#else
	if (creationTime)
	{
		creationTime->SetUnixTimestamp(s.st_ctim.tv_sec);
	}
	if (lastAccessTime)
	{
		lastAccessTime->SetUnixTimestamp(s.st_atim.tv_sec);
	}
	if (lastWriteTime)
	{
		lastWriteTime->SetUnixTimestamp(s.st_mtim.tv_sec);
	}
#endif
}

void IO::FileStream::SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
	if (handle == 0)
		return;
	ClassData *clsData = (ClassData*)this->handle;
	struct utimbuf t;
	if (lastAccessTime == 0 || lastWriteTime == 0)
	{
#if defined(__USE_LARGEFILE64)
		struct stat64 s;
		if (stat64((const Char*)clsData->fileName, &s) != 0)
			return;
#else
		struct stat s;
		if (stat((const Char*)clsData->fileName, &s) != 0)
			return;
#endif
#if defined(__APPLE__)
		t.actime = s.st_atimespec.tv_sec;
		t.modtime = s.st_mtimespec.tv_sec;
#else
		t.actime = s.st_atim.tv_sec;
		t.modtime = s.st_mtim.tv_sec;
#endif
	}
	if (lastAccessTime)
	{
		t.actime = lastAccessTime->ToUnixTimestamp();
	}
	if (lastWriteTime)
	{
		t.modtime = lastWriteTime->ToUnixTimestamp();
	}
	utime((const Char*)clsData->fileName, &t);
}

