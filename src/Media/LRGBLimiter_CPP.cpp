#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/LRGBLimiter.h"

extern "C" void LRGBLimiter_LimitImageLRGB(UInt8 * imgPtr, UIntOS w, UIntOS h)
{
	UIntOS pxCnt = w * h;
	Int16 minV;
	Int16 maxV;
	Int16 v;
	while (pxCnt-- > 0)
	{
		minV = ReadLInt16(&imgPtr[0]);
		maxV = minV;
		v = ReadLInt16(&imgPtr[2]);
		if (v > maxV) maxV = v;
		if (v < minV) minV = v;
		v = ReadLInt16(&imgPtr[4]);
		if (v > maxV) maxV = v;
		if (v < minV) minV = v;

		if (minV >= 16384)
		{
			WriteLInt16(&imgPtr[0], 16383);
			WriteLInt16(&imgPtr[2], 16383);
			WriteLInt16(&imgPtr[4], 16383);
		}
		else if (maxV >= 16384)
		{
			v = ReadLInt16(&imgPtr[0]);
			WriteLInt16(&imgPtr[0], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
			v = ReadLInt16(&imgPtr[2]);
			WriteLInt16(&imgPtr[2], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
			v = ReadLInt16(&imgPtr[4]);
			WriteLInt16(&imgPtr[4], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
		}
		imgPtr += 8;
	}
}
