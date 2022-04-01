#include "Stdafx.h"
#include "MemTool.h"

void MemTool_Init()
{
}


void __stdcall MyMemCopy0(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
}

void __stdcall MyMemCopy1(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*destPtr = *srcPtr;
}

void __stdcall MyMemCopy2(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int16*)destPtr = *(Int16*)srcPtr;
}

void __stdcall MyMemCopy3(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int16*)destPtr = *(Int16*)srcPtr;
	destPtr[2] = srcPtr[2];
}

void __stdcall MyMemCopy4(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
}

void __stdcall MyMemCopy5(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	destPtr[4] = srcPtr[4];
}

void __stdcall MyMemCopy6(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	*(Int16*)&destPtr[4] = *(Int16*)&srcPtr[4];
}

void __stdcall MyMemCopy7(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int32*)destPtr = *(Int32*)srcPtr;
	*(Int16*)&destPtr[4] = *(Int16*)&srcPtr[4];
	destPtr[6] = srcPtr[6];
}

void __stdcall MyMemCopy8(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
}

void __stdcall MyMemCopy9(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	destPtr[8] = srcPtr[8];
}

void __stdcall MyMemCopy10(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int16*)&destPtr[8] = *(Int16*)&srcPtr[8];
}

void __stdcall MyMemCopy11(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int16*)&destPtr[8] = *(Int16*)&srcPtr[8];
	destPtr[10] = srcPtr[10];
}

void __stdcall MyMemCopy12(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
}

void __stdcall MyMemCopy13(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	destPtr[12] = srcPtr[12];
}

void __stdcall MyMemCopy14(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	*(Int16*)&destPtr[12] = *(Int16*)&srcPtr[12];
}

void __stdcall MyMemCopy15(UInt8 *destPtr, const UInt8 *srcPtr, UOSInt size)
{
	*(Int64*)destPtr = *(Int64*)srcPtr;
	*(Int32*)&destPtr[8] = *(Int32*)&srcPtr[8];
	*(Int16*)&destPtr[12] = *(Int16*)&srcPtr[12];
	destPtr[14] = srcPtr[14];
}