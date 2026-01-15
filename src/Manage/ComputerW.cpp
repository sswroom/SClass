#include "Stdafx.h"
#include "Manage/Computer.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

UnsafeArrayOpt<UTF8Char> Manage::Computer::GetHostName(UnsafeArray<UTF8Char> sbuff)
{
	WChar tmpBuff[1024];
	DWORD size = 1024;
	if (GetComputerNameW(tmpBuff, &size))
	{
		return Text::StrWChar_UTF8(sbuff, tmpBuff);
	}
	return nullptr;
}
