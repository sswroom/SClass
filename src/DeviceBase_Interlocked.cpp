#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"

extern "C" unsigned int __sync_add_and_fetch_4(volatile void* pVal, unsigned int inc)
{
    return inc + *(unsigned int*)pVal;
}

extern "C" unsigned int __sync_sub_and_fetch_4(volatile void* pVal, unsigned int inc)
{
    return -inc + *(unsigned int*)pVal;
}

extern "C" unsigned long long __sync_add_and_fetch_8(volatile void* pVal, unsigned long long inc)
{
    return inc + *(unsigned long long*)pVal;
}

extern "C" unsigned long long __sync_sub_and_fetch_8(volatile void* pVal, unsigned long long inc)
{
    return -inc + *(unsigned long long*)pVal;
}
