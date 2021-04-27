#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ModuleInfo.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define PSAPI_VERSION 1
#include <psapi.h>
#undef GetModuleFileName

Manage::ModuleInfo::ModuleInfo(void *hProc, void *hMod)
{
	this->hProc = hProc;
	this->hMod = hMod;
}

Manage::ModuleInfo::~ModuleInfo()
{
}

UTF8Char *Manage::ModuleInfo::GetModuleFileName(UTF8Char *buff)
{
	WChar sbuff[512];
	*sbuff = 0;
#ifdef _WIN32_WCE
	GetModuleFileNameW((HMODULE)this->hMod, sbuff, 512 * sizeof(WChar));
#else
	GetModuleFileNameExW((HANDLE)this->hProc, (HMODULE)this->hMod, sbuff, 512 * sizeof(WChar));
#endif
	return Text::StrWChar_UTF8(buff, sbuff, -1);
}

Bool Manage::ModuleInfo::GetModuleAddress(UOSInt *baseAddr, UOSInt *size)
{
	MODULEINFO modInfo;
	if (GetModuleInformation((HANDLE)this->hProc, (HMODULE)this->hMod, &modInfo, sizeof(MODULEINFO)))
	{
		*baseAddr = (UOSInt)modInfo.lpBaseOfDll;
		*size = modInfo.SizeOfImage;
		return true;
	}
	else
	{
		*baseAddr = 0;
		*size = 0;
		return false;
	}
}
