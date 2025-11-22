#include "Stdafx.h"
#include "Core/ByteTool_C.h"

extern "C" UOSInt ImageTo8Bit_CalDiff(Int32 col1, Int32 col2)
{
	UInt8 c1[4];
	UInt8 c2[4];
	WriteInt32(c1, col1);
	WriteInt32(c2, col2);
	Int32 v1 = c1[0] - (Int32)c2[0];
	UOSInt sum = 0;
	if (v1 < 0)
		sum += (UInt32)-v1;
	else
		sum += (UInt32)v1;
	v1 = c1[1] - (Int32)c2[1];
	if (v1 < 0)
		sum += (UInt32)-v1;
	else
		sum += (UInt32)v1;
	v1 = c1[2] - (Int32)c2[2];
	if (v1 < 0)
		sum += (UInt32)-v1;
	else
		sum += (UInt32)v1;
	return sum;
}
