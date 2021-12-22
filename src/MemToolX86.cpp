#include "Stdafx.h"
#include "MemTool.h"
#include "Manage/CPUInfo.h"
#include "Manage/CPUInst.h"

#if defined(CPU_X86_64)
extern "C"
{
	void MemClearANC_SSE(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_SSE(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_SSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyANC_SSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, UOSInt leng);

	void MemClearANC_AVX(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AVX(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyANC_AVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, UOSInt leng);

	void MemClearANC_AMDSSE(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDSSE(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyANC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNAC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNANC_AMDSSE(void *destPtr, const void *srcPtr, UOSInt leng);

	void MemClearANC_AMDAVX(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDAVX(void *buff, UOSInt buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyANC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNAC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng);
	void MemCopyNANC_AMDAVX(void *destPtr, const void *srcPtr, UOSInt leng);
}

MemClearFunc MemClearANC = MemClearANC_SSE;
MemClearFunc MemClearAC = MemClearAC_SSE;
MemCopyFunc MemCopyAC = MemCopyAC_SSE;
MemCopyFunc MemCopyANC = MemCopyANC_SSE;
MemCopyFunc MemCopyNAC = MemCopyNAC_SSE;
MemCopyFunc MemCopyNANC = MemCopyNANC_SSE;

extern "C"
{
	Int32 UseAVX = 0;
	Int32 UseSSE42 = 0;
	Int32 CPUBrand = 0;
}
#elif defined(CPU_X86_32)
extern "C"
{
	Int32 UseAVX = 0;
	Int32 UseSSE42 = 0;
	Int32 CPUBrand = 0;
}
#endif

void MemTool_Init()
{
#if defined(CPU_X86_64)
		Manage::CPUInfo cpuInfo;
		CPUBrand = (Int32)cpuInfo.GetBrand();
		if (CPUBrand == 2)
		{
			if (Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX2))
			{
				UseAVX = 1;
				MemClearANC = MemClearANC_AMDAVX;
				MemClearAC = MemClearAC_AMDAVX;
				MemCopyAC = MemCopyAC_AMDAVX;
				MemCopyANC = MemCopyANC_AMDAVX;
				MemCopyNAC = MemCopyNAC_AMDAVX;
				MemCopyNANC = MemCopyNANC_AMDAVX;
			}
			else
			{
				MemClearANC = MemClearANC_AMDSSE;
				MemClearAC = MemClearAC_AMDSSE;
				MemCopyAC = MemCopyAC_AMDSSE;
				MemCopyANC = MemCopyANC_AMDSSE;
				MemCopyNAC = MemCopyNAC_AMDSSE;
				MemCopyNANC = MemCopyNANC_AMDSSE;
			}
		}
		else
		{
			if (Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX) && Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX2))
			{
				UseAVX = 1;
				MemClearANC = MemClearANC_AVX;
				MemClearAC = MemClearAC_AVX;
				MemCopyAC = MemCopyAC_AVX;
				MemCopyANC = MemCopyANC_AVX;
				MemCopyNAC = MemCopyNAC_AVX;
				MemCopyNANC = MemCopyNANC_AVX;
			}
		}
		if (Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_SSE42))
		{
			UseSSE42 = 1;
		}
#elif defined(CPU_X86_32)
	Manage::CPUInfo cpuInfo;
	CPUBrand = (Int32)cpuInfo.GetBrand();
	if (CPUBrand == 2)
	{
		if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX2))
		{
			UseAVX = 1;
		}
	}
	else
	{
		if (cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX) && cpuInfo.HasInstruction(Manage::CPUInfo::IT_AVX2))
		{
			UseAVX = 1;
		}
	}
#endif
}
