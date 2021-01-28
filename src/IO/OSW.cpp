#include "Stdafx.h"
#include "IO/OS.h"
#include "Text/MyString.h"
#include <windows.h>

UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	OSVERSIONINFOEX v;
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&v))
	{
		if (v.wProductType == VER_NT_WORKSTATION)
		{
			return Text::StrConcat(sbuff, (const UTF8Char*)"WindowsNT");
		}
		else
		{
			return Text::StrConcat(sbuff, (const UTF8Char*)"Windows Server");
		}
	}
	return 0;
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	OSVERSIONINFOEX v;
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&v))
	{
		sbuff = Text::StrUInt32(sbuff, v.dwMajorVersion);
		*sbuff++ = '.';
		sbuff = Text::StrUInt32(sbuff, v.dwMinorVersion);
		*sbuff++ = '.';
		sbuff = Text::StrUInt32(sbuff, v.dwBuildNumber);
		return sbuff;
	}
	return 0;
}
