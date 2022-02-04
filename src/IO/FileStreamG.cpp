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

IO::FileStream::FileStream(const WChar *fileName, IO::FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(fileName)
{
	handle = 0;
	if (fileName == 0)
	{
		this->currPos = -1;
		handle = 0;
		return;
	}
	else if (*fileName == 0)
	{
		this->currPos = -1;
		handle = 0;
		return;
	}

#ifndef SUPPORT_SHARE
#ifdef _MSC_VER
	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = _wfopen(fileName, L"wb+");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_CREATEWRITE)
	{
		handle = _wfopen(fileName, L"wb");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = _wfopen(fileName, L"ab+");
		if (handle == 0)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = ftell((FILE*)handle);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = _wfopen(fileName, L"rb");
		currPos = 0;
	}
#else
	Char cfileName[512];
	Char *csptr = cfileName;
	while (*csptr++ = *fileName++) ;
	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = fopen(cfileName, "wb+");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_CREATEWRITE)
	{
		handle = fopen(cfileName, "wb");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = fopen(cfileName, "ab+");
		if (handle == 0)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = ftell((FILE*)handle);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = fopen(cfileName, "rb");
		currPos = 0;
	}
#endif
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

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = _wfsopen(fileName, L"wb+", shflag);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_CREATEWRITE)
	{
		handle = _wfsopen(fileName, L"wb", shflag);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = _wfsopen(fileName, L"ab+", shflag);
		if (handle == 0)
		{
			this->currPos = -1;
		}
		else
		{
#if _MSC_VER >= 1400
			this->currPos = _ftelli64((FILE*)handle);
#else
			this->currPos = ftell((FILE*)handle);
#endif
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = _wfsopen(fileName, L"rb", shflag);
		currPos = 0;
	}
#endif
}

IO::FileStream::FileStream(const Char *fileName, FileMode mode, FileShare share, IO::FileStream::BufferType buffType) : IO::SeekableStream(L"FileStream")
{
	handle = 0;
#ifndef SUPPORT_SHARE
	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = fopen(fileName, "wb+");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_CREATEWRITE)
	{
		handle = fopen(fileName, "wb");
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = fopen(fileName, "ab+");
		if (handle)
		{
			this->currPos = ftell((FILE*)handle);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = fopen(fileName, "rb");
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

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = _fsopen(fileName, "wb+", shflag);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_CREATEWRITE)
	{
		handle = _fsopen(fileName, "wb", shflag);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = _fsopen(fileName, "ab+", shflag);
		if (handle)
		{
#if _MSC_VER >= 1400
			this->currPos = _ftelli64((FILE*)handle);
#else
			this->currPos = ftell((FILE*)handle);
#endif
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = _fsopen(fileName, "rb", shflag);
		currPos = 0;
	}
#endif
}

IO::FileStream::~FileStream()
{
	Close();
}

Bool IO::FileStream::IsError()
{
	return this->handle == 0;
}

OSInt IO::FileStream::Read(UInt8 *buff, OSInt size)
{
	if (handle == 0)
		return 0;
	OSInt readSize = fread(buff, 1, size, (FILE*)handle);
	this->currPos += readSize;
	return readSize;
}

OSInt IO::FileStream::Write(const UInt8 *buff, OSInt size)
{
	if (handle == 0)
		return 0;
	OSInt readSize = fwrite(buff, 1, size, (FILE*)handle);
	this->currPos += readSize;
	return readSize;
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

Int64 IO::FileStream::Seek(IO::SeekableStream::SeekType origin, Int64 position)
{
	if (handle == 0)
		return -1;
#if _MSC_VER >= 1400 && defined(SUPPORT_SHARE)
	if (origin == Stream::Begin)
	{
		_fseeki64((FILE*)handle, position, SEEK_SET);
		this->currPos = _ftelli64((FILE*)handle);
		return this->currPos;
	}
	else if (origin == Stream::Current)
	{
		_fseeki64((FILE*)handle, position, SEEK_CUR);
		this->currPos = _ftelli64((FILE*)handle);
		return this->currPos;
	}
	else if (origin == Stream::End)
	{
		_fseeki64((FILE*)handle, position, SEEK_END);
		this->currPos = _ftelli64((FILE*)handle);
		return this->currPos;
	}
#else
	if (origin == IO::SeekableStream::ST_BEGIN)
	{
		fseek((FILE*)handle, (Int32)position, SEEK_SET);
		this->currPos = ftell((FILE*)handle);
		return this->currPos;
	}
	else if (origin == IO::SeekableStream::ST_CURRENT)
	{
		fseek((FILE*)handle, (Int32)position, SEEK_CUR);
		this->currPos = ftell((FILE*)handle);
		return this->currPos;
	}
	else if (origin == IO::SeekableStream::ST_END)
	{
		fseek((FILE*)handle, (Int32)position, SEEK_END);
		this->currPos = ftell((FILE*)handle);
		return this->currPos;
	}
#endif
	return -1;
}

Int64 IO::FileStream::GetPosition()
{
	return currPos;
}

#ifdef _MSC_VER
#ifndef SUPPORT_SHARE
Int64 IO::FileStream::GetLength()
{
	Int32 hand = (Int32)_fileno((FILE*)this->handle);
	return _filelengthi64(hand);
}
#else
Int64 IO::FileStream::GetLength()
{
	Int32 hand = (Int32)_fileno((FILE*)this->handle);
	return _filelengthi64(hand);
}
#endif
#else
Int64 IO::FileStream::GetLength()
{
	Int64 pos = this->currPos;
	Int64 leng = Seek(IO::SeekableStream::ST_END, 0);
	Seek(IO::SeekableStream::ST_BEGIN, pos);
	return leng;
}
#endif

void IO::FileStream::SetLength(Int64 newLength)
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

UOSInt IO::FileStream::LoadFile(const UTF8Char *fileName, UInt8 *buff, UOSInt maxBuffSize)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, FileMode::ReadOnly, FileShare::DenyNone, BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	UInt64 fileLen = fs->GetLength();
	if (fileLen > maxBuffSize || fileLen == 0)
	{
		DEL_CLASS(fs);
		return 0;
	}
	UOSInt readSize = fs->Read(buff, maxBuffSize);
	DEL_CLASS(fs);
	if (readSize == fileLen)
	{
		return readSize;
	}
	return 0;
}
