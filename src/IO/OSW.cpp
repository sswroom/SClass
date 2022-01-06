#include "Stdafx.h"
#include "IO/OS.h"
#include "IO/Path.h"
#include "Manage/EnvironmentVar.h"
#include "Text/MyString.h"
#include <windows.h>


static Bool OS_VersionLoaded = false;
static UInt32 OS_MajorVersion = 0;
static UInt32 OS_MinorVersion = 0;
static UInt32 OS_BuildNumber = 0;

void OS_LoadVersion()
{
	UTF8Char sbuff[512];
	UTF8Char* sptr;
	if (!OS_VersionLoaded)
	{
		OS_VersionLoaded = true;

		sptr = Manage::EnvironmentVar::GetEnvValue(sbuff, (const UTF8Char*)"SystemRoot");
		sptr = Text::StrConcatC(sptr, UTF8STRC("\\System32\\kernel32.dll"));
		DWORD dwDummy;
		DWORD dwFVISize = GetFileVersionInfoSizeA((LPCSTR)sbuff, &dwDummy);
		LPBYTE lpVersionInfo = MemAlloc(BYTE, dwFVISize);
		if (GetFileVersionInfoA((LPCSTR)sbuff, 0, dwFVISize, lpVersionInfo) == 0)
		{
			MemFree(lpVersionInfo);
			return;
		}

		UINT uLen;
		VS_FIXEDFILEINFO* lpFfi;
		BOOL bVer = VerQueryValue(lpVersionInfo, L"\\", (LPVOID*)&lpFfi, &uLen);

		if (!bVer || uLen == 0)
		{
			MemFree(lpVersionInfo);
			return;
		}
		OS_MajorVersion = HIWORD(lpFfi->dwProductVersionMS);
		OS_MinorVersion = LOWORD(lpFfi->dwProductVersionMS);
		OS_BuildNumber = HIWORD(lpFfi->dwProductVersionLS);
		MemFree(lpVersionInfo);
	}
}

UTF8Char *IO::OS::GetDistro(UTF8Char *sbuff)
{
	OSVERSIONINFOEX v;
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&v))
	{
		if (v.wProductType == VER_NT_WORKSTATION)
		{
			return Text::StrConcatC(sbuff, UTF8STRC("WindowsNT"));
		}
		else
		{
			return Text::StrConcatC(sbuff, UTF8STRC("Windows Server"));
		}
	}
	return 0;
}

UTF8Char *IO::OS::GetVersion(UTF8Char *sbuff)
{
	OS_LoadVersion();
	sbuff = Text::StrUInt32(sbuff, OS_MajorVersion);
	*sbuff++ = '.';
	sbuff = Text::StrUInt32(sbuff, OS_MinorVersion);
	*sbuff++ = '.';
	sbuff = Text::StrUInt32(sbuff, OS_BuildNumber);
	return sbuff;
}

UOSInt IO::OS::GetBuildNumber()
{
	OS_LoadVersion();
	return OS_BuildNumber;

}