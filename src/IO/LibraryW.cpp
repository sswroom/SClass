#include "Stdafx.h"
#include "IO/Library.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#ifdef _WIN32_WCE
#include "Text/StringBuilder.hpp"
#endif
#include <windows.h>

IO::Library::Library(UnsafeArray<const UTF8Char> fileName)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(fileName);
	this->hModule = LoadLibraryW(wptr.Ptr());
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

void *IO::Library::GetFunc(UnsafeArray<const Char> funcName)
{
#ifndef _WIN32_WCE
	return (void*)GetProcAddress((HMODULE)this->hModule, funcName.Ptr());
#else
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(UnsafeArray<const UTF8Char>::ConvertFrom(funcName));
	void *ptr = (void*)GetProcAddressW((HMODULE)this->hModule, wptr.Ptr());
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
