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
			UInt16x4 aiVal = PSUBUW4(PUInt16x4SetA(0xFFFF), aVal);
			Int16x4 sVal = PLoadInt16x4(&rgbTable[262144 + src[2] * 8]);
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[264192 + src[1] * 8]));
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[266240 + src[0] * 8]));
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[268288 + src[3] * 8]));
			sVal = PCONVU16x4_I(PMULUHW4(PCONVI16x4_U(sVal), aVal));

			Int16x4 dVal = PLoadInt16x4(&rgbTable[270336 + dest[2] * 8]);
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[272384 + dest[1] * 8]));
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[274432 + dest[0] * 8]));
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[276480 + dest[3] * 8]));
			dVal = PCONVU16x4_I(PMULUHW4(PCONVI16x4_U(dVal), aiVal));
			dVal = PSADDW4(dVal, sVal);

			dest[0] = rgbTable[PEXTW4(dVal, 0)];
			dest[1] = rgbTable[PEXTW4(dVal, 1) + 65536];
			dest[2] = rgbTable[PEXTW4(dVal, 2) + 131072];
			dest[3] = rgbTable[PEXTW4(dVal, 3) + 196608];

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
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			UInt16x4 aVal = PUInt16x4SetA((UInt16)((src[3] << 8) | src[3]));
			UInt16x4 aiVal = PSUBUW4(PUInt16x4SetA(0xFFFF), aVal);
			Int16x4 sVal = PLoadInt16x4(&rgbTable[262144 + src[2] * 8]);
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[264192 + src[1] * 8]));
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[266240 + src[0] * 8]));
			sVal = PSADDW4(sVal, PLoadInt16x4(&rgbTable[268288 + src[3] * 8]));

			Int16x4 dVal = PLoadInt16x4(&rgbTable[270336 + dest[2] * 8]);
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[272384 + dest[1] * 8]));
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[274432 + dest[0] * 8]));
			dVal = PSADDW4(dVal, PLoadInt16x4(&rgbTable[276480 + dest[3] * 8]));
			dVal = PCONVU16x4_I(PMULUHW4(PCONVI16x4_U(dVal), aiVal));
			dVal = PSADDW4(dVal, sVal);

			dest[0] = rgbTable[PEXTW4(dVal, 0)];
			dest[1] = rgbTable[PEXTW4(dVal, 1) + 65536];
			dest[2] = rgbTable[PEXTW4(dVal, 2) + 131072];
			dest[3] = rgbTable[PEXTW4(dVal, 3) + 196608];

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}
