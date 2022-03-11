#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/CS/CSRGB8_LRGBC.h"
#include <wchar.h>

void CSRGB8_LRGBC_ConvertPixel(UInt8 *destPtr, UInt8 *rgbTable, UInt8 b, UInt8 g, UInt8 r)
{
	Int32 outColor[4];
	outColor[0] = *(Int16*)&rgbTable[b * 8 + 4096 + 0];
	outColor[1] = *(Int16*)&rgbTable[b * 8 + 4096 + 2];
	outColor[2] = *(Int16*)&rgbTable[b * 8 + 4096 + 4];
	outColor[3] = *(Int16*)&rgbTable[b * 8 + 4096 + 6];
	outColor[0] += *(Int16*)&rgbTable[g * 8 + 2048 + 0];
	outColor[1] += *(Int16*)&rgbTable[g * 8 + 2048 + 2];
	outColor[2] += *(Int16*)&rgbTable[g * 8 + 2048 + 4];
	outColor[3] += *(Int16*)&rgbTable[g * 8 + 2048 + 6];
	outColor[0] += *(Int16*)&rgbTable[r * 8 + 0 + 0];
	outColor[1] += *(Int16*)&rgbTable[r * 8 + 0 + 2];
	outColor[2] += *(Int16*)&rgbTable[r * 8 + 0 + 4];
	outColor[3] += *(Int16*)&rgbTable[r * 8 + 0 + 6];
	if (outColor[0] < -32768)
	{
		*(Int16*)&destPtr[0] = -32768;
	}
	else if (outColor[0] > 32767)
	{
		*(Int16*)&destPtr[0] = 32767;
	}
	else
	{
		*(Int16*)&destPtr[0] = (Int16)outColor[0];
	}
	if (outColor[1] < -32768)
	{
		*(Int16*)&destPtr[2] = -32768;
	}
	else if (outColor[1] > 32767)
	{
		*(Int16*)&destPtr[2] = 32767;
	}
	else
	{
		*(Int16*)&destPtr[2] = (Int16)outColor[1];
	}
	if (outColor[2] < -32768)
	{
		*(Int16*)&destPtr[4] = -32768;
	}
	else if (outColor[2] > 32767)
	{
		*(Int16*)&destPtr[4] = 32767;
	}
	else
	{
		*(Int16*)&destPtr[4] = (Int16)outColor[2];
	}
	if (outColor[3] < -32768)
	{
		*(Int16*)&destPtr[6] = -32768;
	}
	else if (outColor[3] > 32767)
	{
		*(Int16*)&destPtr[6] = 32767;
	}
	else
	{
		*(Int16*)&destPtr[6] = (Int16)outColor[3];
	}
}

extern "C" void CSRGB8_LRGBC_UpdateRGBTablePal(UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable, OSInt nColor)
{
	while (nColor-- > 0)
	{
		CSRGB8_LRGBC_ConvertPixel(destPal, rgbTable, srcPal[0], srcPal[1], srcPal[2]);

		srcPal += 4;
		destPal += 8;
	}
}

extern "C" void CSRGB8_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	OSInt i;
	if (srcNBits == 32)
	{
		srcRGBBpl -= width * 4;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, srcPtr[0], srcPtr[1], srcPtr[2]);

				srcPtr += 4;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 24)
	{
		srcRGBBpl -= width * 3;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, srcPtr[0], srcPtr[1], srcPtr[2]);

				srcPtr += 3;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 16)
	{
		srcRGBBpl -= width * 2;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				UInt16 c = ReadUInt16(&srcPtr[0]) & 0x7fff;
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, (UInt8)(((c << 3) & 0xf8) | ((c >> 2) & 7)), (UInt8)(((c >> 2) & 0xf8) | ((c >> 7) & 7)), (UInt8)(((c >> 7) & 0xf8) | (c >> 12)));

				srcPtr += 2;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 8)
	{
		srcRGBBpl -= width;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				*(Int64*)destPtr = *(Int64*)&destPal[srcPtr[0] * 8];

				srcPtr += 1;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 4)
	{
		srcRGBBpl -= width >> 1;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width >> 1;
			while (i-- > 0)
			{
				*(Int64*)&destPtr[0] = *(Int64*)&destPal[(srcPtr[0] >> 4) * 8];
				*(Int64*)&destPtr[8] = *(Int64*)&destPal[(srcPtr[0] & 15) * 8];

				srcPtr += 1;
				destPtr += 16;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 2)
	{
		srcRGBBpl -= width >> 2;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width >> 2;
			while (i-- > 0)
			{
				*(Int64*)&destPtr[0] = *(Int64*)&destPal[(srcPtr[0] >> 6) * 8];
				*(Int64*)&destPtr[8] = *(Int64*)&destPal[((srcPtr[0] >> 4) & 3) * 8];
				*(Int64*)&destPtr[16] = *(Int64*)&destPal[((srcPtr[0] >> 2) & 3) * 8];
				*(Int64*)&destPtr[24] = *(Int64*)&destPal[(srcPtr[0] & 3) * 8];

				srcPtr += 1;
				destPtr += 32;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
	else if (srcNBits == 1)
	{
		srcRGBBpl -= width >> 3;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width >> 3;
			while (i-- > 0)
			{
				*(Int64*)&destPtr[0] = *(Int64*)&destPal[((srcPtr[0] >> 7) & 1) * 8];
				*(Int64*)&destPtr[8] = *(Int64*)&destPal[((srcPtr[0] >> 6) & 1) * 8];
				*(Int64*)&destPtr[16] = *(Int64*)&destPal[((srcPtr[0] >> 5) & 1) * 8];
				*(Int64*)&destPtr[24] = *(Int64*)&destPal[((srcPtr[0] >> 4) & 1) * 8];
				*(Int64*)&destPtr[32] = *(Int64*)&destPal[((srcPtr[0] >> 3) & 1) * 8];
				*(Int64*)&destPtr[40] = *(Int64*)&destPal[((srcPtr[0] >> 2) & 1) * 8];
				*(Int64*)&destPtr[48] = *(Int64*)&destPal[((srcPtr[0] >> 1) & 1) * 8];
				*(Int64*)&destPtr[56] = *(Int64*)&destPal[((srcPtr[0] >> 0) & 1) * 8];

				srcPtr += 1;
				destPtr += 64;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertW8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	if (srcNBits == 16)
	{
		OSInt i;
		srcRGBBpl -= width * 2;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, srcPtr[0], srcPtr[0], srcPtr[0]);

				srcPtr += 2;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertB5G5R5(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	if (srcNBits == 16)
	{
		OSInt i;
		srcRGBBpl -= width * 2;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				UInt16 c = ReadUInt16(&srcPtr[0]) & 0x7fff;
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, (UInt8)(((c << 3) & 0xf8) | ((c >> 2) & 7)), (UInt8)(((c >> 2) & 0xf8) | ((c >> 7) & 7)), (UInt8)(((c >> 7) & 0xf8) | (c >> 12)));

				srcPtr += 2;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertB5G6R5(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	if (srcNBits == 16)
	{
		OSInt i;
		srcRGBBpl -= width * 2;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				UInt16 c = ReadUInt16(&srcPtr[0]);
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, (UInt8)(((c << 3) & 0xf8) | ((c >> 2) & 7)), (UInt8)(((c >> 3) & 0xfc) | ((c >> 9) & 3)), (UInt8)(((c >> 7) & 0xf8) | (c >> 13)));

				srcPtr += 2;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertR8G8B8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	if (srcNBits == 24)
	{
		OSInt i;
		srcRGBBpl -= width * 3;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, srcPtr[2], srcPtr[1], srcPtr[0]);

				srcPtr += 3;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertR8G8B8A8(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	if (srcNBits == 32)
	{
		OSInt i;
		srcRGBBpl -= width * 4;
		destRGBBpl -= width * 8;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				CSRGB8_LRGBC_ConvertPixel(destPtr, rgbTable, srcPtr[2], srcPtr[1], srcPtr[0]);

				srcPtr += 4;
				destPtr += 8;
			}
			srcPtr += srcRGBBpl;
			destPtr += destRGBBpl;
		}
	}
}

extern "C" void CSRGB8_LRGBC_ConvertP1_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	OSInt i;
	UInt8 *dptr;
	static UInt16 aVal[2] = {0, 16383};
	while (height-- > 0)
	{
		dptr = destPtr;
		i = width >> 3;
		while (i-- > 0)
		{
			*(Int64*)&dptr[0] = *(Int64*)&destPal[((srcPtr[0] >> 7) & 1) * 8];
			*(Int64*)&dptr[8] = *(Int64*)&destPal[((srcPtr[0] >> 6) & 1) * 8];
			*(Int64*)&dptr[16] = *(Int64*)&destPal[((srcPtr[0] >> 5) & 1) * 8];
			*(Int64*)&dptr[24] = *(Int64*)&destPal[((srcPtr[0] >> 4) & 1) * 8];
			*(Int64*)&dptr[32] = *(Int64*)&destPal[((srcPtr[0] >> 3) & 1) * 8];
			*(Int64*)&dptr[40] = *(Int64*)&destPal[((srcPtr[0] >> 2) & 1) * 8];
			*(Int64*)&dptr[48] = *(Int64*)&destPal[((srcPtr[0] >> 1) & 1) * 8];
			*(Int64*)&dptr[56] = *(Int64*)&destPal[((srcPtr[0] >> 0) & 1) * 8];

			srcPtr += 1;
			dptr += 64;
		}
		if (width & 7)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 7;
			while (i-- > 0)
			{
				*(Int64*)&dptr[0] = *(Int64*)&destPal[((v >> 7) & 1) * 8];
				v = v << 1;
				dptr += 8;
			}
		}

		dptr = destPtr;
		i = width >> 3;
		while (i-- > 0)
		{
			*(UInt16*)&dptr[6] = aVal[(srcPtr[0] >> 7) & 1];
			*(UInt16*)&dptr[14] = aVal[(srcPtr[0] >> 6) & 1];
			*(UInt16*)&dptr[22] = aVal[(srcPtr[0] >> 5) & 1];
			*(UInt16*)&dptr[30] = aVal[(srcPtr[0] >> 4) & 1];
			*(UInt16*)&dptr[38] = aVal[(srcPtr[0] >> 3) & 1];
			*(UInt16*)&dptr[46] = aVal[(srcPtr[0] >> 2) & 1];
			*(UInt16*)&dptr[54] = aVal[(srcPtr[0] >> 1) & 1];
			*(UInt16*)&dptr[62] = aVal[(srcPtr[0] >> 0) & 1];

			srcPtr += 1;
			dptr += 64;
		}
		if (width & 7)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 7;
			while (i-- > 0)
			{
				*(UInt16*)&dptr[6] = aVal[(v >> 7) & 1];
				v = v << 1;
				dptr += 8;
			}
		}

		destPtr += destRGBBpl;
	}
}

extern "C" void CSRGB8_LRGBC_ConvertP2_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	OSInt i;
	UInt8 *dptr;
	static UInt16 aVal[2] = {0, 16383};
	while (height-- > 0)
	{
		dptr = destPtr;
		i = width >> 2;
		while (i-- > 0)
		{
			*(Int64*)&dptr[0] = *(Int64*)&destPal[(srcPtr[0] >> 6) * 8];
			*(Int64*)&dptr[8] = *(Int64*)&destPal[((srcPtr[0] >> 4) & 3) * 8];
			*(Int64*)&dptr[16] = *(Int64*)&destPal[((srcPtr[0] >> 2) & 3) * 8];
			*(Int64*)&dptr[24] = *(Int64*)&destPal[(srcPtr[0] & 3) * 8];

			srcPtr += 1;
			dptr += 32;
		}
		if (width & 3)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 3;
			while (i-- > 0)
			{
				*(Int64*)&dptr[0] = *(Int64*)&destPal[(srcPtr[0] >> 6) * 8];
				v = v << 2;
				dptr += 8;
			}
		}

		dptr = destPtr;
		i = width >> 3;
		while (i-- > 0)
		{
			*(UInt16*)&dptr[6] = aVal[(srcPtr[0] >> 7) & 1];
			*(UInt16*)&dptr[14] = aVal[(srcPtr[0] >> 6) & 1];
			*(UInt16*)&dptr[22] = aVal[(srcPtr[0] >> 5) & 1];
			*(UInt16*)&dptr[30] = aVal[(srcPtr[0] >> 4) & 1];
			*(UInt16*)&dptr[38] = aVal[(srcPtr[0] >> 3) & 1];
			*(UInt16*)&dptr[46] = aVal[(srcPtr[0] >> 2) & 1];
			*(UInt16*)&dptr[54] = aVal[(srcPtr[0] >> 1) & 1];
			*(UInt16*)&dptr[62] = aVal[(srcPtr[0] >> 0) & 1];

			srcPtr += 1;
			dptr += 64;
		}
		if (width & 7)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 7;
			while (i-- > 0)
			{
				*(UInt16*)&dptr[6] = aVal[(v >> 7) & 1];
				v = v << 1;
				dptr += 8;
			}
		}

		destPtr += destRGBBpl;
	}
}

extern "C" void CSRGB8_LRGBC_ConvertP4_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	OSInt i;
	UInt8 *dptr;
	static UInt16 aVal[2] = {0, 16383};
	while (height-- > 0)
	{
		dptr = destPtr;
		i = width >> 1;
		while (i-- > 0)
		{
			*(Int64*)&dptr[0] = *(Int64*)&destPal[(srcPtr[0] >> 4) * 8];
			*(Int64*)&dptr[8] = *(Int64*)&destPal[(srcPtr[0] & 15) * 8];

			srcPtr += 1;
			dptr += 16;
		}
		if (width & 1)
		{
			*(Int64*)&dptr[0] = *(Int64*)&destPal[(srcPtr[0] >> 4) * 8];
			srcPtr += 1;
			dptr += 8;
		}

		dptr = destPtr;
		i = width >> 3;
		while (i-- > 0)
		{
			*(UInt16*)&dptr[6] = aVal[(srcPtr[0] >> 7) & 1];
			*(UInt16*)&dptr[14] = aVal[(srcPtr[0] >> 6) & 1];
			*(UInt16*)&dptr[22] = aVal[(srcPtr[0] >> 5) & 1];
			*(UInt16*)&dptr[30] = aVal[(srcPtr[0] >> 4) & 1];
			*(UInt16*)&dptr[38] = aVal[(srcPtr[0] >> 3) & 1];
			*(UInt16*)&dptr[46] = aVal[(srcPtr[0] >> 2) & 1];
			*(UInt16*)&dptr[54] = aVal[(srcPtr[0] >> 1) & 1];
			*(UInt16*)&dptr[62] = aVal[(srcPtr[0] >> 0) & 1];

			srcPtr += 1;
			dptr += 64;
		}
		if (width & 7)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 7;
			while (i-- > 0)
			{
				*(UInt16*)&dptr[6] = aVal[(v >> 7) & 1];
				v = v << 1;
				dptr += 8;
			}
		}

		destPtr += destRGBBpl;
	}
}

extern "C" void CSRGB8_LRGBC_ConvertP8_A1(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, UInt8 *srcPal, UInt8 *destPal, UInt8 *rgbTable)
{
	OSInt i;
	UInt8 *dptr;
	static UInt16 aVal[2] = {0, 16383};
	while (height-- > 0)
	{
		dptr = destPtr;
		i = width;
		while (i-- > 0)
		{
			*(Int64*)dptr = *(Int64*)&destPal[srcPtr[0] * 8];

			srcPtr += 1;
			dptr += 8;
		}

		dptr = destPtr;
		i = width >> 3;
		while (i-- > 0)
		{
			*(UInt16*)&dptr[6] = aVal[(srcPtr[0] >> 7) & 1];
			*(UInt16*)&dptr[14] = aVal[(srcPtr[0] >> 6) & 1];
			*(UInt16*)&dptr[22] = aVal[(srcPtr[0] >> 5) & 1];
			*(UInt16*)&dptr[30] = aVal[(srcPtr[0] >> 4) & 1];
			*(UInt16*)&dptr[38] = aVal[(srcPtr[0] >> 3) & 1];
			*(UInt16*)&dptr[46] = aVal[(srcPtr[0] >> 2) & 1];
			*(UInt16*)&dptr[54] = aVal[(srcPtr[0] >> 1) & 1];
			*(UInt16*)&dptr[62] = aVal[(srcPtr[0] >> 0) & 1];

			srcPtr += 1;
			dptr += 64;
		}
		if (width & 7)
		{
			UInt8 v = srcPtr[0];
			srcPtr++;
			i = width & 7;
			while (i-- > 0)
			{
				*(UInt16*)&dptr[6] = aVal[(v >> 7) & 1];
				v = v << 1;
				dptr += 8;
			}
		}

		destPtr += destRGBBpl;
	}
}
