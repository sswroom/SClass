#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <cstdlib>
#include <cxxabi.h>

void *operator new(unsigned int size)
{
	return MemAlloc(UInt8, size);
}

void operator delete(void *p)
{
	MemFree(p);
}

extern "C" void __cxa_pure_virtual()
{
	abort();
}

