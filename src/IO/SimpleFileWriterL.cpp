#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SimpleFileWriter.h"
#include "Text/MyString.h"
#include <stdio.h>

UOSInt IO::SimpleFileWriter::WriteBuff(UnsafeArray<const UInt8> buff, UOSInt size)
{
	if (handle == 0)
		return 0;
	UOSInt writeSize = fwrite(buff.Ptr(), 1, size, (FILE*)handle);
	return writeSize;
}

IO::SimpleFileWriter::SimpleFileWriter(UnsafeArray<const UTF8Char> fileName, IO::FileMode mode, IO::FileShare share)
{
	handle = 0;
	const Char *cfileName = (const Char*)fileName.Ptr();
	if (mode == IO::FileMode::Create)
	{
		handle = fopen(cfileName, "wb+");
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = fopen(cfileName, "wb");
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = fopen(cfileName, "ab+");
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = fopen(cfileName, "rb");
	}
}

IO::SimpleFileWriter::~SimpleFileWriter()
{
	if (handle)
	{
		fclose((FILE*)handle);
		handle = 0;
	}
}

Bool IO::SimpleFileWriter::Write(Text::CStringNN str)
{
	UOSInt writeCnt;
	writeCnt = WriteBuff(str.v, str.leng);
	return writeCnt == str.leng;
}

Bool IO::SimpleFileWriter::WriteLine(Text::CStringNN str)
{
	UOSInt writeCnt;
	UInt8 buff[2];
	buff[0] = 13;
	buff[1] = 10;
	writeCnt = WriteBuff(str.v, str.leng);
	writeCnt += WriteBuff(buff, 2);
	return writeCnt == str.leng + 2;
}

Bool IO::SimpleFileWriter::WriteW(UnsafeArray<const WChar> str, UOSInt nChar)
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
	return writeCnt == nChar;
}

Bool IO::SimpleFileWriter::WriteW(UnsafeArray<const WChar> str)
{
	UOSInt writeCnt = 0;
	UOSInt charCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while ((*bptr++ = (UInt8)*str++) != 0)
		if ((bptr - buff) >= 512)
		{
			charCnt += (UOSInt)(bptr - buff);
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	charCnt += (UOSInt)(bptr - buff - 1);
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff - 1));
	return charCnt == writeCnt;
}

Bool IO::SimpleFileWriter::WriteLineW(UnsafeArray<const WChar> str, UOSInt nChar)
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
	return writeCnt == nChar + 2;
}

Bool IO::SimpleFileWriter::WriteLineW(UnsafeArray<const WChar> str)
{
	UOSInt writeCnt = 0;
	UOSInt charCnt = 0;
	UInt8 buff[512];
	UInt8 *bptr;

	bptr = buff;
	while ((*bptr++ = (UInt8)*str++) != 0)
		if ((bptr - buff) >= 510)
		{
			charCnt += (UOSInt)(bptr - buff);
			writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff));
			bptr = buff;
		}
	bptr[-1] = 13;
	bptr[0] = 10;
	charCnt += (UOSInt)(bptr - buff + 1);
	writeCnt += WriteBuff(buff, (UOSInt)(bptr - buff + 1));
	return writeCnt == charCnt;
}

Bool IO::SimpleFileWriter::WriteLine()
{
	UInt8 buff[2];
	buff[0] = 13;
	buff[1] = 10;
	return WriteBuff(buff, 2) == 2;
}

