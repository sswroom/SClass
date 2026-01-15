#include "Stdafx.h"
#include "Manage/ComputerUser.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

UnsafeArrayOpt<UTF8Char> Manage::ComputerUser::GetProcessUser(UnsafeArray<UTF8Char> sbuff)
{
	WChar tmpBuff[1024];
	DWORD size = 1024;
	if (GetUserNameW(tmpBuff, &size))
	{
		return Text::StrWChar_UTF8(sbuff, tmpBuff);
	}
	return nullptr;
}
