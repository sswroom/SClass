#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

IO::DebugWriter::DebugWriter()
{
}

IO::DebugWriter::~DebugWriter()
{
}

Bool IO::DebugWriter::Write(const UTF8Char *str, UOSInt nChar)
{

	UOSInt strLen = Text::StrUTF8_WCharCnt(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(wstr, str, nChar, 0);
	wstr[strLen] = 0;
	OutputDebugStringW(wstr);
	MemFree(wstr);
	return true;
}

Bool IO::DebugWriter::Write(const UTF8Char *str)
{
	const WChar *wstr = Text::StrToWCharNew(str);
	OutputDebugStringW(wstr);
	Text::StrDelNew(wstr);
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 3);
	Text::StrUTF8_WChar(wstr, str, nChar, 0);
	wstr[strLen] = 13;
	wstr[strLen] = 10;
	wstr[strLen] = 0;
	OutputDebugStringW(wstr);
	MemFree(wstr);
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(str, -1);
	WChar *wstr = MemAlloc(WChar, strLen + 3);
	Text::StrUTF8_WChar(wstr, str, -1, 0);
	wstr[strLen] = 13;
	wstr[strLen] = 10;
	wstr[strLen] = 0;
	OutputDebugStringW(wstr);
	MemFree(wstr);
	return true;
}

Bool IO::DebugWriter::WriteW(const WChar *str, UOSInt nChar)
{
	WChar sbuff[257];
	while (nChar >= 256)
	{
		MemCopyNO(sbuff, str, sizeof(WChar) * 256);
		str = &str[256];
		sbuff[256] = 0;
		OutputDebugStringW(sbuff);
		nChar -= 256;
	}
	if (nChar)
	{
		MemCopyNO(sbuff, str, nChar << 1);
		sbuff[nChar] = 0;
		OutputDebugStringW(sbuff);
	}
	return true;
}

Bool IO::DebugWriter::WriteW(const WChar *str)
{
	OutputDebugStringW(str);
	return true;
}

Bool IO::DebugWriter::WriteLineW(const WChar *str, UOSInt nChar)
{
	WChar sbuff[259];
	while (nChar >= 256)
	{
		MemCopyNO(sbuff, str, sizeof(WChar) * 256);
		str = &str[256];
		sbuff[256] = 0;
		OutputDebugStringW(sbuff);
		nChar -= 256;
	}
	if (nChar)
	{
		MemCopyNO(sbuff, str, nChar << 1);
		sbuff[nChar] = 13;
		sbuff[nChar + 1] = 10;
		sbuff[nChar + 2] = 0;
		OutputDebugStringW(sbuff);
	}
	else
	{
		OutputDebugStringW(L"\r\n");
	}
	return true;
}

Bool IO::DebugWriter::WriteLineW(const WChar *str)
{
	OutputDebugStringW(str);
	OutputDebugStringW(L"\r\n");
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	OutputDebugStringW(L"\r\n");
	return true;
}
