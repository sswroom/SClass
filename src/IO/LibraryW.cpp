#include "Stdafx.h"
#include "IO/Library.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#ifdef _WIN32_WCE
#include "Text/StringBuilder.h"
#endif
#include <windows.h>

IO::Library::Library(const UTF8Char *fileName)
{
	const WChar *wptr = Text::StrToWCharNew(fileName);
	this->hModule = LoadLibraryW(wptr);
	Text::StrDelNew(wptr);
}

IO::Library::~Library()
{
	if (this->hModule)
	{
		FreeLibrary((HMODULE)this->hModule);
		this->hModule = 0;
	}
}

Bool IO::Library::IsError()
{
	return this->hModule == 0;
}

void *IO::Library::GetFunc(const Char *funcName)
{
#ifndef _WIN32_WCE
	return (void*)GetProcAddress((HMODULE)this->hModule, funcName);
#else
	const WChar *wptr = Text::StrToWCharNew((const UTF8Char*)funcName);
	void *ptr = (void*)GetProcAddressW((HMODULE)this->hModule, wptr);
	Text::StrDelNew(wptr);
	return ptr;
#endif
}

void *IO::Library::GetFuncNum(OSInt num)
{
#if defined(_WIN32_WCE)
	return (void*)GetProcAddress((HMODULE)this->hModule, (LPCWSTR)num);
#else
	return (void*)GetProcAddress((HMODULE)this->hModule, (LPCSTR)num);
#endif
}
