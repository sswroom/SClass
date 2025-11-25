#include "Stdafx.h"
#include "Core/X86Util_C.h"
#include "Manage/CPUInst.h"

Bool Manage::CPUInst::HasInstruction(InstructionType instType)
{
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	Int32 cpuInfo[4];
	switch (instType)
	{
	case IT_X86:
		return true;
	case IT_SSE41:
		X86Util_cpuid(cpuInfo, 1);
		return (cpuInfo[2] & 0x80000) != 0;
	case IT_SSE42:
		X86Util_cpuid(cpuInfo, 1);
		return (cpuInfo[2] & 0x100000) != 0;
	case IT_AVX:
		X86Util_cpuid(cpuInfo, 1);
		return (cpuInfo[2] & 0x10000000) != 0;
	case IT_AVX2:
		X86Util_cpuidex(cpuInfo, 7, 0);
		return (cpuInfo[1] & 0x20) != 0;
	default:
		return false;
	}
#else
	return false;
#endif
}

