#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileVersion.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

typedef struct
{
	WORD wLanguage;
	WORD wCodePage;
} LANGANDCODEPAGE;

IO::FileVersion::FileVersion(UInt8 *buff, UInt32 buffSize)
{
	this->buff = buff;
	this->buffSize = buffSize;
}

IO::FileVersion::~FileVersion()
{
	MemFree(this->buff);
}

Int32 IO::FileVersion::GetFirstLang()
{
	LANGANDCODEPAGE *langs;
	UInt32 valCnt = 0;
	VerQueryValueW(this->buff, L"\\VarFileInfo\\Translation", (LPVOID*)&langs, &valCnt);
	if (valCnt > 0)
		return (langs[0].wLanguage << 16) | langs[0].wCodePage;
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::FileVersion::GetInternalName(Int32 lang, UnsafeArray<UTF8Char> sbuff)
{
	WChar wbuff2[256];
	UnsafeArray<WChar> wptr;
	wptr = Text::StrConcat(wbuff2, L"\\StringFileInfo\\");
	wptr = Text::StrHexVal32(wptr, lang);
	wptr = Text::StrConcat(wptr, L"\\InternalName");

	UInt32 size;
	if (VerQueryValueW(this->buff, wbuff2, (LPVOID*)&wptr, &size) != 0)
	{
		return Text::StrWChar_UTF8(sbuff, wptr);
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::FileVersion::GetFileDescription(Int32 lang, UnsafeArray<UTF8Char> sbuff)
{
	WChar wbuff2[256];
	UnsafeArray<WChar> wptr;
	wptr = Text::StrConcat(wbuff2, L"\\StringFileInfo\\");
	wptr = Text::StrHexVal32(wptr, lang);
	wptr = Text::StrConcat(wptr, L"\\FileDescription");

	UInt32 size;
	if (VerQueryValueW(this->buff, wbuff2, (LPVOID*)&wptr, &size) != 0)
	{
		return Text::StrWChar_UTF8(sbuff, wptr);
	}
	return 0;
}

IO::FileVersion *IO::FileVersion::Open(const UTF8Char *file)
{
	DWORD dwSize;
	UTF8Char sbuff[512];
	UInt8 *buff;
	UnsafeArray<const WChar> fileName;
	if (file == 0)
	{
		IO::Path::GetProcessFileName(sbuff);
		fileName = Text::StrToWCharNew(sbuff);;
	}
	else
	{
		fileName = Text::StrToWCharNew(file);
	}
	dwSize = GetFileVersionInfoSizeW(fileName.Ptr(), 0);
	if (dwSize > 0)
	{
		buff = MemAlloc(UInt8, dwSize);
		if (GetFileVersionInfoW(fileName.Ptr(), 0, dwSize, buff) != 0)
		{
			IO::FileVersion *ver;
			NEW_CLASS(ver, IO::FileVersion(buff, dwSize));
			Text::StrDelNew(fileName);
			return ver;
		}
		else
		{
			MemFree(buff);
			Text::StrDelNew(fileName);
			return 0;
		}
	}
	Text::StrDelNew(fileName);
	return 0;
}
