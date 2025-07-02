#include "Stdafx.h"
#include "Data/DWGUtil.h"

void Data::DWGUtil::HeaderDecrypt(UnsafeArray<const UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt size)
{
	Int32 randseed = 1;
	UOSInt i = 0;
	while (i < size)
	{
		randseed *= 0x343fd;
		randseed += 0x269ec3;
		destPtr[i] = srcPtr[i] ^ (UInt8)(randseed >> 0x10);
		i++;
	}
}

