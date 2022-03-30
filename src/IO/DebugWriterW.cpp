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

Bool IO::DebugWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{

	UOSInt strLen = Text::StrUTF8_WCharCntC(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WCharC(wstr, str, nChar, 0);
	wstr[strLen] = 0;
	OutputDebugStringW(wstr);
	MemFree(wstr);
	return true;
}

Bool IO::DebugWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
{
	UOSInt strLen = Text::StrUTF8_WCharCntC(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 3);
	Text::StrUTF8_WCharC(wstr, str, nChar, 0);
	wstr[strLen] = 13;
	wstr[strLen + 1] = 10;
	wstr[strLen + 2] = 0;
	OutputDebugStringW(wstr);
	MemFree(wstr);
	return true;
}

/*Bool IO::DebugWriter::WriteW(const WChar *str, UOSInt nChar)
{
	WChar wbuff[257];
	while (nChar >= 256)
	{
		MemCopyNO(wbuff, str, sizeof(WChar) * 256);
		str = &str[256];
		wbuff[256] = 0;
		OutputDebugStringW(wbuff);
		nChar -= 256;
	}
	if (nChar)
	{
		MemCopyNO(wbuff, str, nChar << 1);
		wbuff[nChar] = 0;
		OutputDebugStringW(wbuff);
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
	WChar wbuff[259];
	while (nChar >= 256)
	{
		MemCopyNO(wbuff, str, sizeof(WChar) * 256);
		str = &str[256];
		wbuff[256] = 0;
		OutputDebugStringW(wbuff);
		nChar -= 256;
	}
	if (nChar)
	{
		MemCopyNO(wbuff, str, nChar << 1);
		wbuff[nChar] = 13;
		wbuff[nChar + 1] = 10;
		wbuff[nChar + 2] = 0;
		OutputDebugStringW(wbuff);
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
}*/

Bool IO::DebugWriter::WriteLine()
{
	OutputDebugStringW(L"\r\n");
	return true;
}
