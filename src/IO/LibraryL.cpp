#include "Stdafx.h"
#include "IO/Library.h"
#include "Text/MyString.h"
#include <dlfcn.h>

IO::Library::Library(UnsafeArray<const UTF8Char> fileName)
{
	this->hModule = dlopen((const Char*)fileName.Ptr(), RTLD_NOW);
	if (this->hModule)
	{
		dlerror();
	}
}

IO::Library::~Library()
{
	if (this->hModule)
	{
		dlclose(this->hModule);
		this->hModule = 0;
	}
}

Bool IO::Library::IsError()
{
	return this->hModule == 0;
}

void *IO::Library::GetFunc(UnsafeArray<const Char> funcName)
{
	return dlsym(this->hModule, funcName.Ptr());
}

