#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DeviceStream.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>

IO::DeviceStream::DeviceStream(Text::CStringNN devPath) : IO::Stream(devPath)
{
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.bInheritHandle = true;
	attr.lpSecurityDescriptor = 0;

	const WChar *wptr = Text::StrToWCharNew(devPath.v);
	this->hand = CreateFileW(wptr, 0xC0000000, FILE_SHARE_WRITE | FILE_SHARE_READ, &attr, OPEN_EXISTING, 0, 0);
	Text::StrDelNew(wptr);
}

IO::DeviceStream::~DeviceStream()
{
	this->Close();
}

Bool IO::DeviceStream::IsDown() const
{
	return this->hand == 0;
}

UOSInt IO::DeviceStream::Read(const Data::ByteArray &buff)
{
	if (this->hand)
	{
		while (this->hand)
		{
			Sync::SimpleThread::Sleep(100);
		}
		return 0;
/*		UInt32 readSize;
		ReadFile((HANDLE)this->hand, buff, (DWORD)size, (LPDWORD)&readSize, 0);
		return readSize;*/
	}
	else
	{
		return 0;
	}
}

UOSInt IO::DeviceStream::Write(Data::ByteArrayR buff)
{
	if (this->hand)
	{
		UInt32 readSize;
		WriteFile((HANDLE)this->hand, buff.Ptr(), (DWORD)buff.GetSize(), (LPDWORD)&readSize, 0);
		return readSize;
	}
	else
	{
		return 0;
	}
}

Int32 IO::DeviceStream::Flush()
{
	return 0;
}

void IO::DeviceStream::Close()
{
	if (this->hand)
	{
		CloseHandle((HANDLE)this->hand);
		this->hand = 0;
	}
}

Bool IO::DeviceStream::Recover()
{
	this->Close();
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.bInheritHandle = true;
	attr.lpSecurityDescriptor = 0;

	const WChar *wptr = Text::StrToWCharNew(this->sourceName->v);
	this->hand = CreateFileW(wptr, 0xC0000000, FILE_SHARE_WRITE | FILE_SHARE_READ, &attr, OPEN_EXISTING, 0, 0);
	Text::StrDelNew(wptr);
	return true;
}

IO::StreamType IO::DeviceStream::GetStreamType() const
{
	return IO::StreamType::HID;
}
