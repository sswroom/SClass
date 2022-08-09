#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/LRGBLimiter.h"

extern "C" void LRGBLimiter_LimitImageLRGB(UInt8 * imgPtr, UOSInt w, UOSInt h)
{
	UOSInt pxCnt = w * h;
	Int16 minV;
	Int16 maxV;
	Int16 v;
	while (pxCnt-- > 0)
	{
		minV = ReadInt16(&imgPtr[0]);
		maxV = minV;
		v = ReadInt16(&imgPtr[2]);
		if (v > maxV) maxV = v;
		if (v < minV) minV = v;
		v = ReadInt16(&imgPtr[4]);
		if (v > maxV) maxV = v;
		if (v < minV) minV = v;

		if (minV >= 16384)
		{
			WriteInt16(&imgPtr[0], 16383);
			WriteInt16(&imgPtr[2], 16383);
			WriteInt16(&imgPtr[4], 16383);
		}
		else if (maxV >= 16384)
		{
			v = ReadInt16(&imgPtr[0]);
			WriteInt16(&imgPtr[0], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
			v = ReadInt16(&imgPtr[2]);
			WriteInt16(&imgPtr[2], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
			v = ReadInt16(&imgPtr[4]);
			WriteInt16(&imgPtr[4], (v - minV) * (16384 - minV) / (maxV - minV) + minV);
		}
		imgPtr += 8;
	}
}
