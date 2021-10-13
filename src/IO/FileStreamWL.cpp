#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#ifndef _WIN32_WCE
#include <share.h>
#endif

IO::FileStream::FileStream(const WChar *fileName, IO::FileStream::FileMode mode, FileShare share)
{
	handle = (void*)-1;
	if (fileName == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}
	else if (*fileName == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}

	Int32 shflag;
	if (share == IO::FileStream::FileShare::DenyNone)
	{
		shflag = _SH_DENYNO;
	}
	else if (share == IO::FileStream::FileShare::DenyRead)
	{
		shflag = _SH_DENYRD;
	}
	else if (share == IO::FileStream::FileShare::DenyWrite)
	{
		shflag = _SH_DENYWR;
	}
	else
	{
		shflag = _SH_DENYRW;
	}

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = (void*)_wsopen(fileName, _O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = (void*)_wsopen(fileName, _O_BINARY | _O_APPEND | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		if ((Int32)handle == -1)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = _telli64((Int32)handle);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = (void*)_wsopen(fileName, _O_BINARY | _O_RDONLY, shflag, _S_IREAD);
		currPos = 0;
	}
}

IO::FileStream::FileStream(const Char *fileName, IO::FileStream::FileMode mode, FileShare share)
{
	handle = (void*)-1;
	if (fileName == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}
	else if (*fileName == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}

	Int32 shflag;
	if (share == IO::FileStream::FileShare::DenyNone)
	{
		shflag = _SH_DENYNO;
	}
	else if (share == IO::FileStream::FileShare::DenyRead)
	{
		shflag = _SH_DENYRD;
	}
	else if (share == IO::FileStream::FileShare::DenyWrite)
	{
		shflag = _SH_DENYWR;
	}
	else
	{
		shflag = _SH_DENYRW;
	}

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = (void*)_sopen(fileName, _O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = (void*)_sopen(fileName, _O_BINARY | _O_APPEND | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		if ((Int32)handle == -1)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = _telli64((Int32)handle);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = (void*)_sopen(fileName, _O_BINARY | _O_RDONLY, shflag, _S_IREAD);
		currPos = 0;
	}
}

IO::FileStream::~FileStream()
{
	Close();
}

Bool IO::FileStream::IsError()
{
	return (Int32)this->handle == -1;
}

OSInt IO::FileStream::Read(UInt8 *buff, OSInt size)
{
	if ((Int32)handle == -1)
		return 0;
	OSInt readSize = _read((Int32)handle, buff, (UInt32)size);
	this->currPos += readSize;
	return readSize;
}

OSInt IO::FileStream::Write(const UInt8 *buff, OSInt size)
{
	if ((Int32)handle == -1)
		return 0;
	OSInt readSize = _write((Int32)handle, buff, (UInt32)size);
	this->currPos += readSize;
	return readSize;
}

Int32 IO::FileStream::Flush()
{
	if ((Int32)handle == -1)
		return 0;
	return _commit((Int32)handle);
}

void IO::FileStream::Close()
{
	if ((Int32)handle != -1)
	{
		_close((Int32)handle);
		handle = (void*)-1;
	}
}

Int64 IO::FileStream::Seek(IO::SeekableStream::SeekType origin, Int64 position)
{
	if ((Int32)handle == -1)
		return -1;
	if (origin == IO::SeekableStream::ST_BEGIN)
	{
		this->currPos = _lseeki64((Int32)handle, position, SEEK_SET);
		return currPos;
	}
	else if (origin == IO::SeekableStream::ST_CURRENT)
	{
		this->currPos = _lseeki64((Int32)handle, position, SEEK_CUR);
		return this->currPos;
	}
	else if (origin == IO::SeekableStream::ST_END)
	{
		this->currPos = _lseeki64((Int32)handle, position, SEEK_END);
		return this->currPos;
	}
	return -1;
}

Int64 IO::FileStream::GetPosition()
{
	return currPos;
}

Int64 IO::FileStream::GetLength()
{
	return _filelengthi64((Int32)handle);
}

Int32 IO::FileStream::GetErrCode()
{
	if ((Int32)handle != -1)
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
