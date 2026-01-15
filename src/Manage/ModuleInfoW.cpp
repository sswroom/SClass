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

UnsafeArray<UTF8Char> Manage::ModuleInfo::GetModuleFileName(UnsafeArray<UTF8Char> buff)
{
	WChar wbuff[512];
	*wbuff = 0;
#ifdef _WIN32_WCE
	GetModuleFileNameW((HMODULE)this->hMod, wbuff, 512 * sizeof(WChar));
#else
	GetModuleFileNameExW((HANDLE)this->hProc, (HMODULE)this->hMod, wbuff, 512 * sizeof(WChar));
#endif
	return Text::StrWChar_UTF8(buff, wbuff);
}

Bool Manage::ModuleInfo::GetModuleAddress(OutParam<UIntOS> baseAddr, OutParam<UIntOS> size)
{
	MODULEINFO modInfo;
	if (GetModuleInformation((HANDLE)this->hProc, (HMODULE)this->hMod, &modInfo, sizeof(MODULEINFO)))
	{
		baseAddr.Set((UIntOS)modInfo.lpBaseOfDll);
		size.Set(modInfo.SizeOfImage);
		return true;
	}
	else
	{
		baseAddr.Set(0);
		size.Set(0);
		return false;
	}
}
