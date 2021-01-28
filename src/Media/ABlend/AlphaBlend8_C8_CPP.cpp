#include "Stdafx.h"
#include "MyMemory.h"

extern "C" void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;
	UInt32 aiVal;
	UInt32 aVal;
	UInt32 rVal;
	UInt32 gVal;
	UInt32 bVal;
	UInt32 daVal;
	UInt32 drVal;
	UInt32 dgVal;
	UInt32 dbVal;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			aVal = (src[3] << 8) | src[3]; //xmm0
			aiVal = 65535 - aVal; //xmm3
			bVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 0] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 0] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 0] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 0]) * aVal; //xmm1
			gVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 2] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 2] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 2] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 2]) * aVal;
			rVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 4] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 4] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 4] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 4]) * aVal;
			aVal = (*(Int16*)&rgbTable[262144 + src[2] * 8 + 6] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 6] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 6] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 6]) * aVal;

			dbVal = (*(Int16*)&rgbTable[270336 + dest[2] * 8 + 0] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 0] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 0] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 0]) * aiVal; //xmm1
			dgVal = (*(Int16*)&rgbTable[270336 + dest[2] * 8 + 2] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 2] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 2] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 2]) * aiVal;
			drVal = (*(Int16*)&rgbTable[270336 + dest[2] * 8 + 4] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 4] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 4] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 4]) * aiVal;
			daVal = (*(Int16*)&rgbTable[270336 + dest[2] * 8 + 6] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 6] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 6] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 6]) * aiVal;

			aVal = aVal + ((aiVal * ((dest[3] << 8) | dest[3])) >> 16);
			rVal = (rVal >> 16) + (drVal >> 16);
			gVal = (gVal >> 16) + (dgVal >> 16);
			bVal = (bVal >> 16) + (dbVal >> 16); // xmm0 without us
			dest[0] = rgbTable[bVal];
			dest[1] = rgbTable[gVal + 65536];
			dest[2] = rgbTable[rVal + 131072];
			dest[3] = (aVal >> 8) & 0xff;

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}

extern "C" void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;
	UInt32 aiVal;
	UInt32 aVal;
	UInt32 rVal;
	UInt32 gVal;
	UInt32 bVal;
	UInt32 drVal;
	UInt32 dgVal;
	UInt32 dbVal;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			aVal = (src[3] << 8) | src[3]; //xmm0
			aiVal = 65535 - aVal; //xmm3
			rVal = *(UInt16*)&rgbTable[262144 + src[2] * 8 + 4];
			gVal = *(UInt16*)&rgbTable[264192 + src[1] * 8 + 2];
			bVal = *(UInt16*)&rgbTable[266240 + src[0] * 8 + 0]; //xmm1

			drVal = (*(UInt16*)&rgbTable[270336 + dest[2] * 8 + 4]) * aiVal;
			dgVal = (*(UInt16*)&rgbTable[272384 + dest[1] * 8 + 2]) * aiVal;
			dbVal = (*(UInt16*)&rgbTable[274432 + dest[0] * 8 + 0]) * aiVal; //xmm1

			aVal = aVal + ((aiVal * ((dest[3] << 8) | dest[3])) >> 16);
			rVal = rVal + (drVal >> 16);
			gVal = gVal + (dgVal >> 16);
			bVal = bVal + (dbVal >> 16); // xmm0 without us
			dest[0] = rgbTable[bVal];
			dest[1] = rgbTable[gVal + 65536];
			dest[2] = rgbTable[rVal + 131072];
			dest[3] = rgbTable[rVal + 196608];

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}

