#include "Stdafx.h"

extern "C" UInt32 FNV1a_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal)
{
	UInt8 c;
	
	while (buffSize-- > 0)
	{
		c = *buff++;
		currVal = (currVal ^ c) * 16777619;
	}
	return currVal;
}
