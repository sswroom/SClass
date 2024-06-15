#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Text/MyString.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>

#include "Win32/StreamCOM.h"

Win32::StreamCOM::StreamCOM(IStream *stm, Bool toRelease) : IO::SeekableStream(CSTR(""))
{
	this->stm = stm;
	this->toRelease = toRelease;
}

Win32::StreamCOM::~StreamCOM()
{
	if (this->toRelease)
	{
		this->stm->Release();
	}
}

Bool Win32::StreamCOM::IsDown() const
{
	return false;
}

UOSInt Win32::StreamCOM::Read(const Data::ByteArray &buff)
{
	UInt32 cbSize = 0;
	this->stm->Read(buff.Arr().Ptr(), (ULONG)buff.GetSize(), (ULONG*)&cbSize);
	return cbSize;
}

UOSInt Win32::StreamCOM::Write(Data::ByteArrayR buff)
{
	UInt32 cbSize = 0;
	this->stm->Write(buff.Ptr(), (ULONG)buff.GetSize(), (ULONG*)&cbSize);
	return cbSize;
}

Int32 Win32::StreamCOM::Flush()
{
	return this->stm->Commit(0);
}

void Win32::StreamCOM::Close()
{
}

Bool Win32::StreamCOM::Recover()
{
	///////////////////////////////////////
	return false;
}

IO::StreamType Win32::StreamCOM::GetStreamType() const
{
	return IO::StreamType::WindowsCOM;
}

UInt64 Win32::StreamCOM::SeekFromBeginning(UInt64 position)
{
	ULARGE_INTEGER li;
	LARGE_INTEGER li2;
	li.QuadPart = 0;
	li2.QuadPart = (Int64)position;
	this->stm->Seek(li2, STREAM_SEEK_SET, &li);
	return li.QuadPart;
}

UInt64 Win32::StreamCOM::SeekFromCurrent(Int64 position)
{
	ULARGE_INTEGER li;
	LARGE_INTEGER li2;
	li.QuadPart = 0;
	li2.QuadPart = position;
	this->stm->Seek(li2, STREAM_SEEK_CUR, &li);
	return li.QuadPart;
}

UInt64 Win32::StreamCOM::SeekFromEnd(Int64 position)
{
	ULARGE_INTEGER li;
	LARGE_INTEGER li2;
	li.QuadPart = 0;
	li2.QuadPart = position;
	this->stm->Seek(li2, STREAM_SEEK_END, &li);
	return li.QuadPart;
}

UInt64 Win32::StreamCOM::GetPosition()
{
	return SeekFromCurrent(0);
}

UInt64 Win32::StreamCOM::GetLength()
{
	STATSTG stat;
	if (this->stm->Stat(&stat, STATFLAG_NONAME) == S_OK)
	{
		return stat.cbSize.QuadPart;
	}
	return 0;
}
