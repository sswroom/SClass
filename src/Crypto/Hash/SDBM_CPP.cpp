#include "Stdafx.h"

extern "C" UInt32 SDBM_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal)
{
	UInt8 c;
	
	while (buffSize-- > 0)
	{
		c = *buff++;
		currVal = c + (currVal << 6) + (currVal << 16) - currVal;
	}
	return currVal;
}
