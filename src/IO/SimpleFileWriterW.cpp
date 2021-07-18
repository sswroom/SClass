#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SimpleFileWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

UOSInt IO::SimpleFileWriter::WriteBuff(const UInt8 *buff, UOSInt size)
{
	UInt32 writeSize;
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	if (WriteFile(handle, buff, (UInt32)size, (DWORD*)&writeSize, 0))
	{
		return writeSize;
	}
	else
	{
		return 0;
	}
}

IO::SimpleFileWriter::SimpleFileWriter(const UTF8Char *fileName, IO::FileStream::FileMode mode, IO::FileStream::FileShare share)
{
	WChar wfileName[512];
	Text::StrUTF8_WChar(wfileName, fileName, 0);
	handle = INVALID_HANDLE_VALUE;
	SECURITY_ATTRIBUTES secAttr;
	UInt32 shflag;
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor = 0;
	secAttr.bInheritHandle = TRUE;
	if (share == IO::FileStream::FILE_SHARE_DENY_NONE)
	{
		shflag = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (share == IO::FileStream::FILE_SHARE_DENY_READ)
	{
		shflag = FILE_SHARE_WRITE;
	}
	else if (share == IO::FileStream::FILE_SHARE_DENY_WRITE)
	{
		shflag = FILE_SHARE_READ;
	}
	else
	{
		shflag = 0;
	}

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = CreateFileW(wfileName, GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = CreateFileW(wfileName, GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (handle != INVALID_HANDLE_VALUE)
		{
			Int32 fleng = 0;
			SetFilePointer(handle, 0, (PLONG)&fleng, FILE_END);
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = CreateFileW(wfileName, GENERIC_READ, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else if (mode == FileStream::FILE_MODE_READWRITEEXISTING)
	{
		handle = CreateFileW(wfileName, GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else if (mode == FileStream::FILE_MODE_DEVICE)
	{
		handle = CreateFileW(wfileName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	}
}

IO::SimpleFileWriter::~SimpleFileWriter()
{
	CloseHandle((HANDLE)handle);
}

Bool IO::SimpleFileWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	UOSInt writeCnt = WriteBuff(str, nChar);
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::Write(const UTF8Char *str)
{
	UOSInt writeCnt = WriteBuff(str, Text::StrCharCnt(str));
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	UOSInt writeCnt = WriteBuff(str, nChar);
	UInt8 buff[2];
	buff[0] = 13;
	buff[1] = 10;
	writeCnt += WriteBuff(buff, 2);
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteLine(const UTF8Char *str)
{
	UOSInt writeCnt = WriteBuff(str, Text::StrCharCnt(str));
	UInt8 buff[2];
	buff[0] = 13;
	buff[1] = 10;
	writeCnt += WriteBuff(buff, 2);
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteW(const WChar *str, UOSInt nChar)
{
	UOSInt writeCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while (nChar-- > 0)
	{
		*bptr++ = (UInt8)*str++;
		if ((bptr - buff) >= 512)
		{
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	}
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteW(const WChar *str)
{
	UOSInt writeCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while ((*bptr++ = (UInt8)*str++) != 0)
		if ((bptr - buff) >= 512)
		{
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff - 1));
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteLineW(const WChar *str, UOSInt nChar)
{
	UOSInt writeCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while (nChar-- > 0)
	{
		*bptr++ = (UInt8)*str++;
		if ((bptr - buff) >= 510)
		{
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	}
	bptr[0] = 13;
	bptr[1] = 10;
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff + 2));
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteLineW(const WChar *str)
{
	UOSInt writeCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while ((*bptr++ = (UInt8)*str++) != 0)
		if ((bptr - buff) >= 510)
		{
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	bptr[-1] = 13;
	bptr[0] = 10;
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff + 1));
	return writeCnt > 0;
}

Bool IO::SimpleFileWriter::WriteLine()
{
	UInt8 buff[2];
	buff[0] = 13;
	buff[1] = 10;
	return WriteBuff(buff, 2) == 2;
}
