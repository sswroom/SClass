#ifndef _SM_CORE_X86UTIL
#define _SM_CORE_X86UTIL

#if defined(CPU_X86_32) || defined(CPU_X86_64)
#if !defined(__MINGW32__) && (defined(HAS_GCCASM32) || defined(HAS_GCCASM64))
static inline void X86Util_ncpuid(Int32 *eax, Int32 *ebx, Int32 *ecx, Int32 *edx)
{
	asm volatile("cpuid": "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) : "0" (*eax), "2" (*ecx));
}

static inline void X86Util_cpuid(Int32 *cpuInfo, Int32 func)
{
	cpuInfo[0] = func;
	cpuInfo[2] = 0;
	X86Util_ncpuid(&cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
}

static inline void X86Util_cpuidex(Int32 *cpuInfo, Int32 func, Int32 subfunc)
{
	cpuInfo[0] = func;
	cpuInfo[2] = subfunc;
	X86Util_ncpuid(&cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
}
#else
extern "C"
{
	void X86Util_ncpuid(Int32 *cpuInfo, Int32 func, Int32 subfunc);
}
#define X86Util_cpuid(cpuInfo, func) X86Util_ncpuid(cpuInfo, func, 0)
#define X86Util_cpuidex(cpuInfo, func, subfunc) X86Util_ncpuid(cpuInfo, func, subfunc)
#endif
#endif

#endif
