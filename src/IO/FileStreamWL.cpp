#include "Stdafx.h"
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

IO::FileStream::FileStream(Text::CString fileName, IO::FileMode mode, FileShare share, IO::FileStream::BufferType bufferType) : IO::SeekableStream(fileName)
{
	handle = (void*)-1;
	if (fileName.v == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}

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
		handle = (void*)_sopen((const Char*)fileName.v, _O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = (void*)_sopen((const Char*)fileName.v, _O_BINARY | _O_APPEND | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		if ((Int32)handle == -1)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = _telli64((Int32)handle);
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = (void*)_sopen((const Char*)fileName.v, _O_BINARY | _O_RDONLY, shflag, _S_IREAD);
		currPos = 0;
	}
}

IO::FileStream::FileStream(Text::String *fileName, IO::FileMode mode, FileShare share, IO::FileStream::BufferType bufferType) : IO::SeekableStream(fileName)
{
	handle = (void*)-1;
	if (fileName == 0)
	{
		this->currPos = -1;
		handle = (void*)-1;
		return;
	}

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
		handle = (void*)_sopen((const Char*)fileName->v, _O_BINARY | _O_CREAT | _O_TRUNC | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = (void*)_sopen((const Char*)fileName->v, _O_BINARY | _O_APPEND | _O_RDWR, shflag, _S_IREAD | _S_IWRITE);
		if ((Int32)handle == -1)
		{
			this->currPos = -1;
		}
		else
		{
			this->currPos = _telli64((Int32)handle);
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = (void*)_sopen((const Char*)fileName->v, _O_BINARY | _O_RDONLY, shflag, _S_IREAD);
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

UOSInt IO::FileStream::Read(UInt8 *buff, UOSInt size)
{
	if ((Int32)handle == -1)
		return 0;
	OSInt readSize = _read((Int32)handle, buff, (UInt32)size);
	this->currPos += (UOSInt)readSize;
	return (UOSInt)readSize;
}

UOSInt IO::FileStream::Write(const UInt8 *buff, UOSInt size)
{
	if ((Int32)handle == -1)
		return 0;
	OSInt readSize = _write((Int32)handle, buff, (UInt32)size);
	this->currPos += (UOSInt)readSize;
	return (UOSInt)readSize;
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

UInt64 IO::FileStream::SeekFromBeginning(UInt64 position)
{
	if ((Int32)handle == -1)
		return -1;
	this->currPos = (UInt64)_lseeki64((Int32)handle, position, SEEK_SET);
	return currPos;
}

UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	if ((Int32)handle == -1)
		return -1;
	this->currPos = (UInt64)_lseeki64((Int32)handle, position, SEEK_CUR);
	return this->currPos;
}

UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	if ((Int32)handle == -1)
		return -1;
	this->currPos = (UInt64)_lseeki64((Int32)handle, position, SEEK_END);
	return this->currPos;
}

UInt64 IO::FileStream::GetPosition()
{
	return this->currPos;
}

UInt64 IO::FileStream::GetLength()
{
	return (UInt64)_filelengthi64((Int32)handle);
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

UOSInt IO::FileStream::LoadFile(Text::CString fileName, UInt8 *buff, UOSInt maxBuffSize)
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
	UOSInt readSize = fs.Read(buff, maxBuffSize);
	if (readSize == fileLen)
	{
		return readSize;
	}
	return 0;
}
