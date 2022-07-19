#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"

#if _WCHAR_SIZE == 2
#define BigInt_ToString BigInt_ToStringW16
#else
#define BigInt_ToString BigInt_ToStringW32
#endif

extern "C" void BigInt_ByteSwap(const UInt8 *srcBuff, UInt8 *destBuff, UOSInt valSize)
{
	srcBuff += valSize;
	while (valSize-- > 0)
	{
		*destBuff++ = *--srcBuff;
	}
}

extern "C" void BigInt_Neg(UInt8 *valBuff, UOSInt valSize)
{
	UInt8 *valPtr = valBuff;
	UOSInt i = valSize >> 2;
	while (i-- > 0)
	{
		*(Int32*)valPtr = ~*(Int32*)valPtr;
		valPtr += 4;
	}

#if IS_BYTEORDER_LE
	valPtr = valBuff;
	i = valSize >> 2;
	while (i-- > 0)
	{
		if (++*(Int32*)valPtr)
			break;
		valPtr += 4;
	}
#else
	valPtr = valBuff + valSize;
	i = valSize >> 2;
	while (i-- > 0)
	{
		valPtr -= 4;
		if (++*(Int32*)valPtr)
			break;
	}
#endif
}

extern "C" void BigInt_EqualI32(UInt8 *valBuff, Int32 valSize, UOSInt val)
{
	*(Int32*)valBuff = val;
	Int32 t = val >> 31;
	valSize = (valSize >> 2) - 1;
	while (valSize-- > 0)
	{
		valBuff += 4;
		*(Int32*)valBuff = t;
	}
}

extern "C" WChar *BigInt_ToString(WChar *buff, const UInt8 *valArr, UInt8 *tmpArr, UOSInt valSize)
{
	////////////////////////////////////
	return buff;
}
