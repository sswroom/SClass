#include "Stdafx.h"

extern "C" UInt32 DJB2_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 currVal)
{
	UInt8 c;
	
	while (buffSize-- > 0)
	{
		c = *buff++;
		currVal = ((currVal << 5) + currVal) + c; /* hash * 33 + c */
	}
	return currVal;
}
