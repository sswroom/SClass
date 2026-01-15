#include "Stdafx.h"
#include "MemTool.h"
#include "SIMD.h"
#include "Manage/CPUInfo.h"
#include "Manage/CPUInst.h"

#if defined(CPU_X86_64)
extern "C"
{
	void MemClearANC_SSE(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_SSE(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_SSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyANC_SSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, UIntOS leng);
//	void MemCopyOAC_SSE(void *destPtr, const void *srcPtr, UIntOS leng);

	void MemClearANC_AVX(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AVX(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyANC_AVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, UIntOS leng);
//	void MemCopyOAC_AVX(void *destPtr, const void *srcPtr, UIntOS leng);

	void MemClearANC_AMDSSE(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDSSE(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDSSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyANC_AMDSSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNAC_AMDSSE(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNANC_AMDSSE(void *destPtr, const void *srcPtr, UIntOS leng);
//	void MemCopyOAC_AMDSSE(void *destPtr, const void *srcPtr, UIntOS leng);

	void MemClearANC_AMDAVX(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemClearAC_AMDAVX(void *buff, UIntOS buffSize); //buff 16-byte align, buffSize 16 bytes
	void MemCopyAC_AMDAVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyANC_AMDAVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNAC_AMDAVX(void *destPtr, const void *srcPtr, UIntOS leng);
	void MemCopyNANC_AMDAVX(void *destPtr, const void *srcPtr, UIntOS leng);
//	void MemCopyOAC_AMDAVX(void *destPtr, const void *srcPtr, UIntOS leng);
}

MemClearFunc MemClearANC = MemClearANC_SSE;
MemClearFunc MemClearAC = MemClearAC_SSE;
MemCopyFunc MemCopyAC = MemCopyAC_SSE;
MemCopyFunc MemCopyANC = MemCopyANC_SSE;
MemCopyFunc MemCopyNAC = MemCopyNAC_SSE;
MemCopyFunc MemCopyNANC = MemCopyNANC_SSE;
//MemCopyFunc MemCopyOAC = MemCopyOAC_SSE;

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
		MemCopyNAC = MemCopyNAC_AMDSSE;
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
//				MemCopyOAC = MemCopyOAC_AMDAVX;
			}
			else
			{
				MemClearANC = MemClearANC_AMDSSE;
				MemClearAC = MemClearAC_AMDSSE;
				MemCopyAC = MemCopyAC_AMDSSE;
				MemCopyANC = MemCopyANC_AMDSSE;
				MemCopyNAC = MemCopyNAC_AMDSSE;
				MemCopyNANC = MemCopyNANC_AMDSSE;
//				MemCopyOAC = MemCopyOAC_AMDSSE;
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
//				MemCopyOAC = MemCopyOAC_AVX;
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
		if (Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX2))
		{
			UseAVX = 1;
		}
	}
	else
	{
		if (Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX) && Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_AVX2))
		{
			UseAVX = 1;
		}
	}
#endif
}


void __stdcall MyMemCopy0(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
}

void __stdcall MyMemCopy1(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*destPtr = *srcPtr;
}

void __stdcall MyMemCopy2(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int16*)destPtr = *(Int16*)srcPtr;
}

void __stdcall MyMemCopy3(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int16*)destPtr = *(Int16*)srcPtr;
	destPtr[2] = srcPtr[2];
}

void __stdcall MyMemCopy4(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
}

void __stdcall MyMemCopy5(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	destPtr[4] = srcPtr[4];
}

void __stdcall MyMemCopy6(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	*(Int16*)&destPtr[4] = *(Int16*)&srcPtr[4];
}

void __stdcall MyMemCopy7(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	*(Int16*)&destPtr[4] = *(Int16*)&srcPtr[4];
	destPtr[6] = srcPtr[6];
}

void __stdcall MyMemCopy8(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
}

void __stdcall MyMemCopy9(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	destPtr[8] = srcPtr[8];
}

void __stdcall MyMemCopy10(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int16*)&destPtr[8] = *(Int16*)&srcPtr[8];
}

void __stdcall MyMemCopy11(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int16*)&destPtr[8] = *(Int16*)&srcPtr[8];
	destPtr[10] = srcPtr[10];
}

void __stdcall MyMemCopy12(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
}

void __stdcall MyMemCopy13(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	destPtr[12] = srcPtr[12];
}

void __stdcall MyMemCopy14(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	*(Int16*)&destPtr[12] = *(Int16*)&srcPtr[12];
}

void __stdcall MyMemCopy15(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	*(Int16*)&destPtr[12] = *(Int16*)&srcPtr[12];
	destPtr[14] = srcPtr[14];
}


void __stdcall MyMemCopy16(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
}

void __stdcall MyMemCopy17(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	destPtr[16] = srcPtr[16];
}

void __stdcall MyMemCopy18(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int16*)&destPtr[16] = *(Int16*)&srcPtr[16];
}

void __stdcall MyMemCopy19(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int16*)&destPtr[16] = *(Int16*)&srcPtr[16];
	destPtr[18] = srcPtr[18];
}

void __stdcall MyMemCopy20(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int32*)&destPtr[16] = *(Int32*)&srcPtr[16];
}

void __stdcall MyMemCopy21(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int32*)&destPtr[16] = *(Int32*)&srcPtr[16];
	destPtr[20] = srcPtr[20];
}

void __stdcall MyMemCopy22(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int32*)&destPtr[16] = *(Int32*)&srcPtr[16];
	*(Int16*)&destPtr[20] = *(Int16*)&srcPtr[20];
}

void __stdcall MyMemCopy23(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int32*)&destPtr[16] = *(Int32*)&srcPtr[16];
	*(Int16*)&destPtr[20] = *(Int16*)&srcPtr[20];
	destPtr[22] = srcPtr[22];
}

void __stdcall MyMemCopy24(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
}

void __stdcall MyMemCopy25(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	destPtr[24] = srcPtr[24];
}

void __stdcall MyMemCopy26(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int16*)&destPtr[24] = *(Int16*)&srcPtr[24];
}

void __stdcall MyMemCopy27(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int16*)&destPtr[24] = *(Int16*)&srcPtr[24];
	destPtr[26] = srcPtr[26];
}

void __stdcall MyMemCopy28(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int32*)&destPtr[24] = *(Int32*)&srcPtr[24];
}

void __stdcall MyMemCopy29(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int32*)&destPtr[24] = *(Int32*)&srcPtr[24];
	destPtr[28] = srcPtr[28];
}

void __stdcall MyMemCopy30(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int32*)&destPtr[24] = *(Int32*)&srcPtr[24];
	*(Int16*)&destPtr[28] = *(Int16*)&srcPtr[28];
}

void __stdcall MyMemCopy31(UInt8 *destPtr, const UInt8 *srcPtr, UIntOS size)
{
	PStoreUInt32x4(destPtr, PLoadUInt32x4(srcPtr));
	*(Int64*)&destPtr[16] = *(Int64*)&srcPtr[16];
	*(Int32*)&destPtr[24] = *(Int32*)&srcPtr[24];
	*(Int16*)&destPtr[28] = *(Int16*)&srcPtr[28];
	destPtr[30] = srcPtr[30];
}
