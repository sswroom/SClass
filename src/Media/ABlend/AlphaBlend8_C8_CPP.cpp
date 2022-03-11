#include "Stdafx.h"
#include "SIMD.h"

extern "C" void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			UInt16x4 aVal = PUInt16x4SetA((UInt16)((src[3] << 8) | src[3]));
			UInt16x4 aiVal = PSUBW4(PUInt16x4SetA(0xFFFF), aVal);
			UInt16x4 sVal = PLoadUInt16x4(&rgbTable[262144 + src[2] * 8]);
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[264192 + src[1] * 8]));
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[266240 + src[0] * 8]));
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[268288 + src[3] * 8]));
			sVal = PMULUHW4(sVal, aVal);

			UInt16x4 dVal = PLoadUInt16x4(&rgbTable[270336 + dest[2] * 8]);
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[272384 + dest[1] * 8]));
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[274432 + dest[0] * 8]));
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[276480 + dest[3] * 8]));
			dVal = PMULUHW4(dVal, aiVal);
			dVal = PSADDUW4(dVal, sVal);

			dest[0] = rgbTable[PEXTUW4(dVal, 0)];
			dest[1] = rgbTable[PEXTUW4(dVal, 1) + 65536];
			dest[2] = rgbTable[PEXTUW4(dVal, 2) + 131072];
			dest[3] = rgbTable[PEXTUW4(dVal, 3) + 196608];

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}
/*{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;
	UInt32 aiVal;
	UInt32 aVal;
	UInt32 rVal;
	UInt32 gVal;
	UInt32 bVal;
	//UInt32 daVal;
	UInt32 drVal;
	UInt32 dgVal;
	UInt32 dbVal;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			aVal = (UInt32)(src[3] << 8) | src[3]; //xmm0
			aiVal = 65535 - aVal; //xmm3
			bVal = (UInt32)((*(Int16*)&rgbTable[262144 + src[2] * 8 + 0] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 0] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 0] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 0]) * (Int32)aVal); //xmm1
			gVal = (UInt32)((*(Int16*)&rgbTable[262144 + src[2] * 8 + 2] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 2] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 2] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 2]) * (Int32)aVal);
			rVal = (UInt32)((*(Int16*)&rgbTable[262144 + src[2] * 8 + 4] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 4] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 4] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 4]) * (Int32)aVal);
			aVal = (UInt32)((*(Int16*)&rgbTable[262144 + src[2] * 8 + 6] + *(Int16*)&rgbTable[264192 + src[1] * 8 + 6] + *(Int16*)&rgbTable[266240 + src[0] * 8 + 6] + *(Int16*)&rgbTable[268288 + src[3] * 8 + 6]) * (Int32)aVal);

			dbVal = (UInt32)((*(Int16*)&rgbTable[270336 + dest[2] * 8 + 0] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 0] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 0] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 0]) * (Int32)aiVal); //xmm1
			dgVal = (UInt32)((*(Int16*)&rgbTable[270336 + dest[2] * 8 + 2] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 2] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 2] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 2]) * (Int32)aiVal);
			drVal = (UInt32)((*(Int16*)&rgbTable[270336 + dest[2] * 8 + 4] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 4] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 4] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 4]) * (Int32)aiVal);
			//daVal = (UInt32)((*(Int16*)&rgbTable[270336 + dest[2] * 8 + 6] + *(Int16*)&rgbTable[272384 + dest[1] * 8 + 6] + *(Int16*)&rgbTable[274432 + dest[0] * 8 + 6] + *(Int16*)&rgbTable[276480 + dest[3] * 8 + 6]) * (Int32)aiVal);

			aVal = aVal + ((aiVal * ((UInt32)(dest[3] << 8) | dest[3])) >> 16);
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
}*/

extern "C" void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			UInt16x4 aVal = PUInt16x4SetA((UInt16)((src[3] << 8) | src[3]));
			UInt16x4 aiVal = PSUBW4(PUInt16x4SetA(0xFFFF), aVal);
			UInt16x4 sVal = PLoadUInt16x4(&rgbTable[262144 + src[2] * 8]);
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[264192 + src[1] * 8]));
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[266240 + src[0] * 8]));
			sVal = PSADDUW4(sVal, PLoadUInt16x4(&rgbTable[268288 + src[3] * 8]));

			UInt16x4 dVal = PLoadUInt16x4(&rgbTable[270336 + dest[2] * 8]);
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[272384 + dest[1] * 8]));
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[274432 + dest[0] * 8]));
			dVal = PSADDUW4(dVal, PLoadUInt16x4(&rgbTable[276480 + dest[3] * 8]));
			dVal = PMULUHW4(dVal, aiVal);
			dVal = PSADDUW4(dVal, sVal);

			dest[0] = rgbTable[PEXTUW4(dVal, 0)];
			dest[1] = rgbTable[PEXTUW4(dVal, 1) + 65536];
			dest[2] = rgbTable[PEXTUW4(dVal, 2) + 131072];
			dest[3] = rgbTable[PEXTUW4(dVal, 3) + 196608];

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}
/*{
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
			aVal = (UInt32)(src[3] << 8) | src[3]; //xmm0
			aiVal = 65535 - aVal; //xmm3
			rVal = *(UInt16*)&rgbTable[262144 + src[2] * 8 + 4];
			gVal = *(UInt16*)&rgbTable[264192 + src[1] * 8 + 2];
			bVal = *(UInt16*)&rgbTable[266240 + src[0] * 8 + 0]; //xmm1

			drVal = (*(UInt16*)&rgbTable[270336 + dest[2] * 8 + 4]) * aiVal;
			dgVal = (*(UInt16*)&rgbTable[272384 + dest[1] * 8 + 2]) * aiVal;
			dbVal = (*(UInt16*)&rgbTable[274432 + dest[0] * 8 + 0]) * aiVal; //xmm1

			aVal = aVal + ((aiVal * ((UInt32)(dest[3] << 8) | dest[3])) >> 16);
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
}*/
