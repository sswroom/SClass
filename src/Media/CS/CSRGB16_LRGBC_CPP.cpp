#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Data/ByteTool.h"

extern "C" void CSRGB16_LRGBC_ConvertB16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	OSInt i;
	srcRGBBpl -= width << 3;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[0]) * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[4]) * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 8;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertR16G16B16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	OSInt i;
	srcRGBBpl -= width << 3;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[4]) * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[0]) * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 8;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertB16G16R16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	OSInt i;
	srcRGBBpl -= width * 6;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[0]) * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[4]) * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 6;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertR16G16B16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	OSInt i;
	srcRGBBpl -= width * 6;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			cvals = PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[4]) * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[2]) * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[ReadUInt16(&srcPtr[0]) * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 6;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertW16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	UInt32 v;
	OSInt i;
	srcRGBBpl -= width * 4;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v = ReadUInt16(&srcPtr[0]);
			cvals = PLoadInt16x4(&rgbTable[v * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 4;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertW16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	UInt32 v;
	OSInt i;
	srcRGBBpl -= width * 2;
	destRGBBpl -= width << 3;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			v = ReadUInt16(&srcPtr[0]);
			cvals = PLoadInt16x4(&rgbTable[v * 8 + 1048576]);
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v * 8 + 524288]));
			cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v * 8 + 0]));
			PStoreInt16x4(destPtr, cvals);
			srcPtr += 2;
			destPtr += 8;
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
	Int16x4 cvals;
	UInt32 v;
	UInt32 v2;
	OSInt i;
	if (srcNBits == 32)
	{
		srcRGBBpl -= width * 4;
		destRGBBpl -= width << 3;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				v = ReadUInt32(&srcPtr[0]);
				v2 = v & 0x3ff;
				cvals = PLoadInt16x4(&rgbTable[v2 * 8 + 0]);
				v2 = (v >> 10) & 0x3ff;
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v2 * 8 + 524288]));
				v2 = (v >> 20) & 0x3ff;
				cvals = PSADDW4(cvals, PLoadInt16x4(&rgbTable[v2 * 8 + 1048576]));
				PStoreInt16x4(destPtr, cvals);
				srcPtr += 4;
				destPtr += 8;
			}
		}
	}
}

extern "C" void CSRGB16_LRGBC_ConvertFloat(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcNBits, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable)
{
}

