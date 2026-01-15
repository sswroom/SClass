#include "Stdafx.h"
#include "MemTool.h"
#include "SIMD.h"

void MemTool_Init()
{
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