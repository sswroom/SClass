#include "Stdafx.h"

extern "C" UInt32 FNV1_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal)
{
	UInt8 c;
	
	while (buffSize-- > 0)
	{
		c = *buff++;
		currVal = (currVal * 16777619) ^ c;
	}
	return currVal;
}
