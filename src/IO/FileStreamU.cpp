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

void IO::FileStream::InitStream(const WChar *fileName, FileMode mode, FileShare share, BufferType buffType)
{
	int flags = 0;
#if defined(O_BINARY)
	flags |= O_BINARY;
#endif
	mode_t opmode = S_IRUSR | S_IWUSR;

	if (mode == FileMode::ReadOnly)
	{
		flags |= O_RDONLY;
	}
	else if (mode == FileMode::Create)
	{
		flags |= O_CREAT|O_RDWR|O_TRUNC;
	}
	else if (mode == FileMode::Append)
	{
		flags |= O_CREAT|O_RDWR;
	}
	else if (mode == FileMode::CreateWrite)
	{
		flags |= O_CREAT|O_WRONLY;
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
	this->handle = (void*)(OSInt)open64((const Char*)this->sourceName->v.Ptr(), flags, opmode);
#else
	this->handle = (void*)(OSInt)open((const Char*)this->sourceName->v.Ptr(), flags, opmode);
#endif
	if ((OSInt)this->handle == -1)
	{
		this->handle = 0;
	}
	if (mode == FileMode::Append && (OSInt)this->handle > 0)
	{
		this->SeekFromEnd(0);
	}
	else
	{
		this->currPos = 0;
	}
}

IO::FileStream::FileStream(NN<Text::String> fileName, FileMode mode, FileShare share, BufferType buffType) : IO::SeekableStream(fileName)
{
	if (fileName->v[0] == 0)
	{
		this->currPos = 0;
		this->handle = 0;
		return;
	}
	this->handle = 0;
	this->InitStream(0, mode, share, buffType);
}

IO::FileStream::FileStream(Text::CStringNN fileName, FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	this->handle = 0;
	this->InitStream(0, mode, share, buffType);
}

IO::FileStream::~FileStream()
{
	Close();
}

Bool IO::FileStream::IsDown() const
{
	return (OSInt)this->handle == 0;
}

Bool IO::FileStream::IsError() const
{
	return (OSInt)this->handle == 0;
}

UOSInt IO::FileStream::Read(const Data::ByteArray &buff)
{
	if (this->handle == 0)
		return 0;
	OSInt readSize = read((int)(OSInt)this->handle, buff.Arr().Ptr(), buff.GetSize());
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

UOSInt IO::FileStream::Write(UnsafeArray<const UInt8> buff, UOSInt size)
{
	if (this->handle == 0)
		return 0;
	OSInt readSize = write((int)(OSInt)this->handle, buff.Ptr(), size);
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
	return fsync((int)(OSInt)this->handle);
}

void IO::FileStream::Close()
{
	if (this->handle)
	{
		close((int)(OSInt)this->handle);
		this->handle = 0;
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
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek((int)(OSInt)this->handle, (Int64)position, SEEK_SET);
#else
	this->currPos = (UInt64)lseek64((int)(OSInt)this->handle, (Int64)position, SEEK_SET);
#endif
	return this->currPos;
}


UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	if (this->handle == 0)
		return 0;
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek((int)(OSInt)this->handle, position, SEEK_CUR);
#else
	this->currPos = (UInt64)lseek64((int)(OSInt)this->handle, position, SEEK_CUR);
#endif
	return this->currPos;
}


UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	if (this->handle == 0)
		return 0;
#if defined(__FreeBSD__) || defined(__APPLE__)
	this->currPos = (UInt64)lseek((int)(OSInt)this->handle, position, SEEK_END);
#else
	this->currPos = (UInt64)lseek64((int)(OSInt)this->handle, position, SEEK_END);
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
	fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, (off_t)newLength};
	int ret = fcntl((int)(OSInt)this->handle, F_PREALLOCATE, &store);
	if (ret == -1)
	{
		store.fst_flags = F_ALLOCATEALL;
		fcntl((int)(OSInt)this->handle, F_PREALLOCATE, &store);
	}
#elif !defined(__ANDROID__)
	posix_fallocate((int)(OSInt)this->handle, (off_t)this->currPos, (off_t)(newLength - this->currPos));
#endif
}

IO::StreamType IO::FileStream::GetStreamType() const
{
	return IO::StreamType::File;
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
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	if (this->handle == 0)
	{
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
	}
	else
	{
		if (fstat64((int)(OSInt)this->handle, &s) != 0)
			return;
	}
#else
	struct stat s;
	if (this->handle == 0)
	{
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
	}
	else
	{
		if (fstat((int)(OSInt)this->handle, &s) != 0)
			return;
	}
#endif
#if defined(__APPLE__)
	if (creationTime)
	{
		creationTime->SetValue(Data::TimeInstant(s.st_ctimespec.tv_sec, (UInt32)s.st_ctimespec.tv_nsec), 0);
	}
	if (lastAccessTime)
	{
		lastAccessTime->SetValue(Data::TimeInstant(s.st_atimespec.tv_sec, (UInt32)s.st_atimespec.tv_nsec), 0);
	}
	if (lastWriteTime)
	{
		lastWriteTime->SetValue(Data::TimeInstant(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec), 0);
	}
#else
	if (creationTime)
	{
		creationTime->SetValue(Data::TimeInstant(s.st_ctim.tv_sec, (UInt32)s.st_ctim.tv_nsec), 0);
	}
	if (lastAccessTime)
	{
		lastAccessTime->SetValue(Data::TimeInstant(s.st_atim.tv_sec, (UInt32)s.st_atim.tv_nsec), 0);
	}
	if (lastWriteTime)
	{
		lastWriteTime->SetValue(Data::TimeInstant(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec), 0);
	}
#endif
}

void IO::FileStream::GetFileTimes(OptOut<Data::Timestamp> creationTime, OptOut<Data::Timestamp> lastAccessTime, OptOut<Data::Timestamp> lastWriteTime)
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	if (this->handle == 0)
	{
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
	}
	else
	{
		if (fstat64((int)(OSInt)this->handle, &s) != 0)
			return;
	}
#else
	struct stat s;
	if (this->handle == 0)
	{
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
	}
	else
	{
		if (fstat((int)(OSInt)this->handle, &s) != 0)
			return;
	}
#endif
#if defined(__APPLE__)
	if (creationTime.IsNotNull())
	{
		creationTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_ctimespec.tv_sec, (UInt32)s.st_ctimespec.tv_nsec, 0));
	}
	if (lastAccessTime.IsNotNull())
	{
		lastAccessTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_atimespec.tv_sec, (UInt32)s.st_atimespec.tv_nsec, 0));
	}
	if (lastWriteTime.IsNotNull())
	{
		lastWriteTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec, 0));
	}
#else
	if (creationTime.IsNotNull())
	{
		creationTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_ctim.tv_sec, (UInt32)s.st_ctim.tv_nsec, 0));
	}
	if (lastAccessTime.IsNotNull())
	{
		lastAccessTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_atim.tv_sec, (UInt32)s.st_atim.tv_nsec, 0));
	}
	if (lastWriteTime.IsNotNull())
	{
		lastWriteTime.SetNoCheck(Data::Timestamp::FromSecNS(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec, 0));
	}
#endif
}

Data::Timestamp IO::FileStream::GetCreateTime()
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	if (this->handle == 0)
	{
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return Data::Timestamp(0);
	}
	else
	{
		if (fstat64((int)(OSInt)this->handle, &s) != 0)
			return Data::Timestamp(0);
	}
#else
	struct stat s;
	if (this->handle == 0)
	{
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return Data::Timestamp(0);
	}
	else
	{
		if (fstat((int)(OSInt)this->handle, &s) != 0)
			return Data::Timestamp(0);
	}
#endif
#if defined(__APPLE__)
	return Data::Timestamp::FromSecNS(s.st_ctimespec.tv_sec, (UInt32)s.st_ctimespec.tv_nsec, 0);
#else
	return Data::Timestamp::FromSecNS(s.st_ctim.tv_sec, (UInt32)s.st_ctim.tv_nsec, 0);
#endif
}


Data::Timestamp IO::FileStream::GetModifyTime()
{
#if defined(__USE_LARGEFILE64)
	struct stat64 s;
	if (this->handle == 0)
	{
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return Data::Timestamp(0);
	}
	else
	{
		if (fstat64((int)(OSInt)this->handle, &s) != 0)
			return Data::Timestamp(0);
	}
#else
	struct stat s;
	if (this->handle == 0)
	{
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return Data::Timestamp(0);
	}
	else
	{
		if (fstat((int)(OSInt)this->handle, &s) != 0)
			return Data::Timestamp(0);
	}
#endif
#if defined(__APPLE__)
	return Data::Timestamp::FromSecNS(s.st_mtimespec.tv_sec, (UInt32)s.st_mtimespec.tv_nsec, 0);
#else
	return Data::Timestamp::FromSecNS(s.st_mtim.tv_sec, (UInt32)s.st_mtim.tv_nsec, 0);
#endif
}

void IO::FileStream::SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
	struct utimbuf t;
	if (lastAccessTime == 0 || lastWriteTime == 0)
	{
#if defined(__USE_LARGEFILE64)
		struct stat64 s;
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
#else
		struct stat s;
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
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
		t.actime = (time_t)lastAccessTime->ToUnixTimestamp();
	}
	if (lastWriteTime)
	{
		t.modtime = (time_t)lastWriteTime->ToUnixTimestamp();
	}
	utime((const Char*)this->sourceName->v.Ptr(), &t);
}

void IO::FileStream::SetFileTimes(const Data::Timestamp &creationTime, const Data::Timestamp &lastAccessTime, const Data::Timestamp &lastWriteTime)
{
	struct utimbuf t;
	if (lastAccessTime.IsNull() || lastWriteTime.IsNull())
	{
#if defined(__USE_LARGEFILE64)
		struct stat64 s;
		if (stat64((const Char*)this->sourceName->v.Ptr(), &s) != 0)
			return;
#else
		struct stat s;
		if (stat((const Char*)this->sourceName->v.Ptr(), &s) != 0)
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
	if (!lastAccessTime.IsNull())
	{
		t.actime = (time_t)lastAccessTime.ToUnixTimestamp();
	}
	if (!lastWriteTime.IsNull())
	{
		t.modtime = (time_t)lastWriteTime.ToUnixTimestamp();
	}
	utime((const Char*)this->sourceName->v.Ptr(), &t);
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
