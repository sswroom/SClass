#include "Stdafx.h"
#include "Core/X86Util_C.h"
#include <intrin.h>

#if defined(CPU_X86_32) || defined(CPU_X86_64)
extern "C" void X86Util_ncpuid(Int32 * cpuInfo, Int32 func, Int32 subfunc)
{
	__cpuidex(cpuInfo, func, subfunc);
}
#endif