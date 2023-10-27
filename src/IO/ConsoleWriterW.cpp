#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#include <stdio.h>
#ifdef _WIN32_WCE
#define WriteConsoleW WriteFile
#define WriteConsoleA WriteFile
#endif

struct IO::ConsoleWriter::ClassData
{
	HANDLE hand;
	Text::Encoding *enc;
	Sync::Mutex mut;
	Bool autoFlush;
	Bool fileOutput;
};

IO::ConsoleWriter::ConsoleWriter()
{
	NEW_CLASS(this->clsData, ClassData());
#ifdef _WIN32_WCE
	this->clsData->hand = CreateFile(L"CON0", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
#else
	this->clsData->hand = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	this->clsData->enc = 0;
	this->clsData->autoFlush = false;
	this->clsData->fileOutput = false;
	this->bgColor = Text::StandardColor::Black;
	UInt32 mode;
#ifndef _WIN32_WCE
	BOOL ret = GetConsoleMode(this->clsData->hand, (LPDWORD)&mode);
	if (ret == 0)
	{
		this->clsData->fileOutput = true;
		NEW_CLASS(this->clsData->enc, Text::Encoding());
	}
#endif
}

IO::ConsoleWriter::~ConsoleWriter()
{
	if (this->clsData->enc)
	{
		DEL_CLASS(this->clsData->enc);
		this->clsData->enc = 0;
	}
#ifdef _WIN32_WCE
	CloseHandle(this->clsData->hand);
#endif
	DEL_CLASS(this->clsData);
}
/*
Bool IO::ConsoleWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(wstr, str, nChar, 0);
	wstr[strLen] = 0;
	Bool ret = this->WriteW(wstr);
	MemFree(wstr);
	return ret;
}

Bool IO::ConsoleWriter::Write(const UTF8Char *str)
{
	const WChar *wstr = Text::StrToWCharNew(str);
	Bool ret = this->WriteW(wstr);
	Text::StrDelNew(wstr);
	return ret;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	UOSInt strLen = Text::StrUTF8_WCharCnt(str, nChar);
	WChar *wstr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(wstr, str, nChar, 0);
	wstr[strLen] = 0;
	Bool ret = this->WriteLineW(wstr);
	MemFree(wstr);
	return ret;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str)
{
	const WChar *wstr = Text::StrToWCharNew(str);
	Bool ret = this->WriteLineW(wstr);
	Text::StrDelNew(wstr);
	return ret;
}*/

Bool IO::ConsoleWriter::WriteStrC(const UTF8Char *s, UOSInt nUTF8Char)
{
	UInt32 outChars = 0;
	UInt32 nChar;
	if (this->clsData->enc == 0)
	{
		UOSInt strLen = Text::StrUTF8_WCharCntC(s, nUTF8Char);
		WChar *str = MemAlloc(WChar, strLen + 1);
		Text::StrUTF8_WCharC(str, s, nUTF8Char, 0);
		WriteConsoleW(this->clsData->hand, str, nChar = (UInt32)strLen, (LPDWORD)&outChars, 0);
		MemFree(str);
		if (outChars == nChar)
		{
			if (this->clsData->autoFlush)
			{
				FlushFileBuffers(this->clsData->hand);
			}
			return true;
		}
		return false;
	}
	else
	{
		UOSInt nBytes;
		UInt8 *tmpBuff;
		nChar = (UInt32)Text::StrCharCnt(s);
		nBytes = this->clsData->enc->UTF8CountBytesC(s, nChar);
		tmpBuff = MemAlloc(UInt8, nBytes + 1);
		this->clsData->enc->UTF8ToBytesC(tmpBuff, s, nChar);
		if (this->clsData->fileOutput)
		{
			WriteFile(this->clsData->hand, tmpBuff, (UInt32)nBytes, (LPDWORD)&outChars, 0);
		}
		else
		{
			WriteConsoleA(this->clsData->hand, tmpBuff, (UInt32)nBytes, (LPDWORD)&outChars, 0);
		}
		MemFree(tmpBuff);
		if (outChars == (UInt32)nBytes)
		{
			if (this->clsData->autoFlush)
			{
				FlushFileBuffers(this->clsData->hand);
			}
			return true;
		}
		return false;
	}
}

Bool IO::ConsoleWriter::WriteLineC(const UTF8Char *s, UOSInt nUTF8Char)
{
	UInt32 outChars = 0;
	UInt32 nChar;
	if (this->clsData->enc == 0)
	{
		UOSInt strLen = Text::StrUTF8_WCharCntC(s, nUTF8Char);
		WChar *str = MemAlloc(WChar, strLen + 2);
		Text::StrUTF8_WCharC(str, s, nUTF8Char, 0);
#if defined(__CYGWIN__)
		str[strLen] = '\r';
		str[strLen + 1] = '\n';
		WriteConsoleW(this->clsData->hand, str, nChar = (UInt32)(strLen + 2), (LPDWORD)&outChars, 0);
#else
		str[strLen] = '\n';
		WriteConsoleW(this->clsData->hand, str, nChar = (UInt32)(strLen + 1), (LPDWORD)&outChars, 0);
#endif
		MemFree(str);
		if (outChars == nChar)
		{
			if (this->clsData->autoFlush)
			{
				FlushFileBuffers(this->clsData->hand);
			}
			return true;
		}
		return false;
	}
	else
	{
		UOSInt nBytes;
		UInt8 *tmpBuff;
		nChar = (UInt32)Text::StrCharCnt(s);
		nBytes = this->clsData->enc->UTF8CountBytesC(s, nChar) + 1;
		tmpBuff = MemAlloc(UInt8, nBytes + 2);
		this->clsData->enc->UTF8ToBytesC(tmpBuff, s, nChar);
		tmpBuff[nBytes - 1] = '\n';
		if (this->clsData->fileOutput)
		{
			WriteFile(this->clsData->hand, tmpBuff, (UInt32)nBytes, (LPDWORD)&outChars, 0);
		}
		else
		{
#if defined(__CYGWIN__)
			tmpBuff[nBytes - 1] = '\r';
			tmpBuff[nBytes] = '\n';
			WriteConsoleA(this->clsData->hand, tmpBuff, (UInt32)nBytes + 1, (LPDWORD)&outChars, 0);
			nBytes += 1;
#else
			WriteConsoleA(this->clsData->hand, tmpBuff, (UInt32)nBytes, (LPDWORD)&outChars, 0);
#endif
		}
		MemFree(tmpBuff);
		if (outChars == (UInt32)nBytes)
		{
			if (this->clsData->autoFlush)
			{
				FlushFileBuffers(this->clsData->hand);
			}
			return true;
		}
		return false;
	}
}

Bool IO::ConsoleWriter::WriteLine()
{
	UInt32 outChars = 0;
	UInt32 nChar;
	if (this->clsData->fileOutput)
	{
		WriteFile(this->clsData->hand, "\n", nChar = 1, (LPDWORD)&outChars, 0);
	}
	else
	{
#if defined(__CYGWIN__)
		WriteConsoleW(this->clsData->hand, L"\r\n", nChar = 2, (LPDWORD)&outChars, 0);
#else
		WriteConsoleW(this->clsData->hand, L"\n", nChar = 1, (LPDWORD)&outChars, 0);
#endif
	}
	if (outChars == nChar)
	{
		if (this->clsData->autoFlush)
		{
			FlushFileBuffers(this->clsData->hand);
		}
		return true;
	}
	return false;
}

Bool IO::ConsoleWriter::WriteChar(UTF8Char c)
{
	UInt32 outChars = 0;
	WChar buff[2];
	buff[0] = c;
	if (this->clsData->fileOutput)
	{
		WriteFile(this->clsData->hand, buff, 1, (LPDWORD)&outChars, 0);
	}
	else
	{
		WriteConsoleW(this->clsData->hand, buff, 1, (LPDWORD)&outChars, 0);
	}
	if (outChars == 1)
	{
		if (this->clsData->autoFlush)
		{
			FlushFileBuffers(this->clsData->hand);
		}
		return true;
	}
	return false;
}

void IO::ConsoleWriter::SetBGColor(Text::StandardColor bgColor)
{
	this->bgColor = bgColor;
}

void IO::ConsoleWriter::SetTextColor(Text::StandardColor fgColor)
{
#ifndef _WIN32_WCE
	SetConsoleTextAttribute(this->clsData->hand, (UInt16)(((UInt8)fgColor & 0xf) | (((UInt8)this->bgColor & 0xf) << 4)));
#endif
}

void IO::ConsoleWriter::ResetTextColor()
{
	SetBGColor(Text::StandardColor::Black);
	SetTextColor(Text::StandardColor::Gray);
}

UOSInt IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	WChar c;
	UOSInt size = 0;
	while ((c = *str++) != 0)
	{
		if (c < 32)
		{
		}
		else if (c < 0x2000)
		{
			size += 1;
		}
		else if (c < 0xff60)
		{
			size += 2;
		}
		else if (c < 0xffA0)
		{
			size += 1;
		}
		else if (c < 0xffe8)
		{
			size += 2;
		}
		else
		{
			size += 1;
		}
	}
	return size;
}

WChar *IO::ConsoleWriter::ReadLine(WChar *sbuff, UOSInt nChar)
{
#if defined(__CYGWIN__)
	Char buff[512];
	if (fgets(buff, nChar, stdin) == 0)
		return 0;
	else
	{
		return Text::StrUTF8_WChar(sbuff, (UTF8Char*)buff, 0);
	}
#else
	if (fgetws(sbuff, (int)nChar, stdin) == 0)
		return 0;
	else
		return &sbuff[Text::StrCharCnt(sbuff)];
#endif
}

void IO::ConsoleWriter::EnableCPFix(Bool isEnable)
{
#ifndef _WIN32_WCE
	if (isEnable || this->clsData->fileOutput)
	{
		if (this->clsData->enc == 0)
		{
			NEW_CLASS(this->clsData->enc, Text::Encoding(GetConsoleOutputCP()));
		}
	}
	else
	{
		if (this->clsData->enc)
		{
			DEL_CLASS(this->clsData->enc);
			this->clsData->enc = 0;
		}
	}
#endif
}

void IO::ConsoleWriter::SetAutoFlush(Bool autoFlush)
{
	this->clsData->autoFlush = autoFlush;
}

Bool IO::ConsoleWriter::GetConsoleState(IO::ConsoleWriter::ConsoleState *state)
{
#ifdef _WIN32_WCE
	return false;
#else
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(this->clsData->hand, &info) == FALSE)
		return false;
	state->fgColor = (Text::StandardColor)(info.wAttributes & 0xf);
	state->bgColor = (Text::StandardColor)((info.wAttributes >> 4) & 0xf);
	state->currX = (UInt32)info.dwCursorPosition.X;
	state->currY = (UInt32)info.dwCursorPosition.Y;
	state->consoleWidth = (UInt32)info.dwSize.X;
	state->consoleHeight = (UInt32)info.dwSize.Y;
	return true;
#endif
}

Bool IO::ConsoleWriter::SetCursorPos(UInt32 x, Int32 y)
{
#ifdef _WIN32_WCE
	return false;
#else
	COORD tmp;
	tmp.X = (SHORT)x;
	tmp.Y = (SHORT)y;
	return SetConsoleCursorPosition(this->clsData->hand, tmp) != 0;
#endif
}

Bool IO::ConsoleWriter::IsFileOutput()
{
	return this->clsData->fileOutput;
}

void IO::ConsoleWriter::FixWrite(const WChar *str, UOSInt displayWidth)
{
	if (this->clsData->fileOutput || this->clsData->enc == 0)
	{
		return;
	}
	UOSInt width = GetDisplayWidth(str);
	if (width <= displayWidth)
	{
		NotNullPtr<Text::String> s = Text::String::NewNotNull(str);
		this->WriteStr(s->ToCString());
		s->Release();
		while (width < displayWidth)
		{
			width++;
			WriteStrC(UTF8STRC(" "));
		}
		return;
	}

	if (displayWidth < 4)
	{
		WChar wbuff[2];
		width = 0;
		while (true)
		{
			if (width >= displayWidth)
				return ;
			width += GetDisplayCharWidth(*str);
			if (width > displayWidth)
			{
				WriteStrC(UTF8STRC(" "));
				return;
			}
			wbuff[0] = str[0];
			wbuff[1] = 0;
			NotNullPtr<Text::String> s = Text::String::NewNotNull(wbuff);
			this->WriteStr(s->ToCString());
			s->Release();
			str++;
		}
	}
	else
	{
		WChar wbuff[2];
		width = 0;
		while (width < displayWidth - 3)
		{
			width += GetDisplayCharWidth(*str);
			wbuff[0] = str[0];
			wbuff[1] = 0;
			NotNullPtr<Text::String> s = Text::String::NewNotNull(wbuff);
			this->WriteStr(s->ToCString());
			s->Release();
			str++;
		}
		while (width < displayWidth)
		{
			this->WriteStrC(UTF8STRC("."));
			width++;
		}
	}
}

UOSInt IO::ConsoleWriter::GetDisplayWidth(const WChar *str)
{
	if (this->clsData->fileOutput || this->clsData->enc == 0)
	{
		return Text::StrCharCnt(str);
	}
	UOSInt size = 0;
	WChar c;
	while ((c = *str++) != 0)
	{
		if (c >= 32)
			size += GetDisplayCharWidth(c);
	}
	return size;
}

UOSInt IO::ConsoleWriter::GetDisplayCharWidth(WChar c)
{
	UInt8 buff[4];
	UOSInt size = this->clsData->enc->WToBytesC(buff, &c, 1);
	if (size == 1 && buff[0] < 128)
		return 1;

#ifdef _WIN32_WCE
	return 2;
#else
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(this->clsData->hand, &info) == FALSE)
		return 1;
	CONSOLE_SCREEN_BUFFER_INFO info2;
	COORD tmp = {0, 0};
	SetConsoleCursorPosition(this->clsData->hand, tmp);
	DWORD nChar;
	WriteConsoleA(this->clsData->hand, buff, (DWORD)size, &nChar, 0);
	if (GetConsoleScreenBufferInfo(this->clsData->hand, &info2) == FALSE)
	{
		size = 1;
	}
	else
	{
		size = (UOSInt)info2.dwCursorPosition.X;
	}
	SetConsoleCursorPosition(this->clsData->hand, info.dwCursorPosition);
	return size;
#endif
}
