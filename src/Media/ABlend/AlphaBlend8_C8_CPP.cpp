#include "Stdafx.h"
#include "SIMD.h"
#include "Data/ByteTool.h"

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
	if (false)
	{
		while (height-- != 0)
		{
			i = width;
			while (i-- != 0)
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
	else if (width & 1)
	{
		width = width >> 1;
		while (height-- != 0)
		{
			i = width;
			while (i-- != 0)
			{
				UInt16x4 aVal1 = PUInt16x4SetA((UInt16)(0xffff - ((src[3] << 8) | src[3])));
				UInt16x4 aVal2 = PUInt16x4SetA((UInt16)(0xffff - ((src[7] << 8) | src[7])));
				UInt16x8 aiVal = PMergeW4(aVal1, aVal2);
				Int16x8 sVal = PMLoadInt16x4(&rgbTable[262144 + src[6] * 8], &rgbTable[262144 + src[2] * 8]);
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[264192 + src[5] * 8], &rgbTable[264192 + src[1] * 8]));
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[266240 + src[4] * 8], &rgbTable[266240 + src[0] * 8]));
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[268288 + src[7] * 8], &rgbTable[268288 + src[3] * 8]));

				Int16x8 dVal = PMLoadInt16x4(&rgbTable[270336 + dest[6] * 8], &rgbTable[270336 + dest[2] * 8]);
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[272384 + dest[5] * 8], &rgbTable[272384 + dest[1] * 8]));
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[274432 + dest[4] * 8], &rgbTable[274432 + dest[0] * 8]));
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[276480 + dest[7] * 8], &rgbTable[276480 + dest[3] * 8]));
				dVal = PCONVU16x8_I(PMULUHW8(PCONVI16x8_U(dVal), aiVal));
				dVal = PSADDW8(dVal, sVal);

				dest[0] = rgbTable[PEXTW8(dVal, 0)];
				dest[1] = rgbTable[PEXTW8(dVal, 1) + 65536];
				dest[2] = rgbTable[PEXTW8(dVal, 2) + 131072];
				dest[3] = rgbTable[PEXTW8(dVal, 3) + 196608];
				dest[4] = rgbTable[PEXTW8(dVal, 4)];
				dest[5] = rgbTable[PEXTW8(dVal, 5) + 65536];
				dest[6] = rgbTable[PEXTW8(dVal, 6) + 131072];
				dest[7] = rgbTable[PEXTW8(dVal, 7) + 196608];

				src += 8;
				dest += 8;
			}
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

			src += sAdd;
			dest += dAdd;
		}
	}
	else
	{
		width = width >> 1;
		while (height-- != 0)
		{
			i = width;
			while (i-- != 0)
			{
				UInt16x4 aVal1 = PUInt16x4SetA((UInt16)(0xffff - ((src[3] << 8) | src[3])));
				UInt16x4 aVal2 = PUInt16x4SetA((UInt16)(0xffff - ((src[7] << 8) | src[7])));
				UInt16x8 aiVal = PMergeW4(aVal1, aVal2);
				Int16x8 sVal = PMLoadInt16x4(&rgbTable[262144 + src[6] * 8], &rgbTable[262144 + src[2] * 8]);
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[264192 + src[5] * 8], &rgbTable[264192 + src[1] * 8]));
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[266240 + src[4] * 8], &rgbTable[266240 + src[0] * 8]));
				sVal = PSADDW8(sVal, PMLoadInt16x4(&rgbTable[268288 + src[7] * 8], &rgbTable[268288 + src[3] * 8]));

				Int16x8 dVal = PMLoadInt16x4(&rgbTable[270336 + dest[6] * 8], &rgbTable[270336 + dest[2] * 8]);
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[272384 + dest[5] * 8], &rgbTable[272384 + dest[1] * 8]));
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[274432 + dest[4] * 8], &rgbTable[274432 + dest[0] * 8]));
				dVal = PSADDW8(dVal, PMLoadInt16x4(&rgbTable[276480 + dest[7] * 8], &rgbTable[276480 + dest[3] * 8]));
				dVal = PCONVU16x8_I(PMULUHW8(PCONVI16x8_U(dVal), aiVal));
				dVal = PSADDW8(dVal, sVal);

				dest[0] = rgbTable[PEXTW8(dVal, 0)];
				dest[1] = rgbTable[PEXTW8(dVal, 1) + 65536];
				dest[2] = rgbTable[PEXTW8(dVal, 2) + 131072];
				dest[3] = rgbTable[PEXTW8(dVal, 3) + 196608];
				dest[4] = rgbTable[PEXTW8(dVal, 4)];
				dest[5] = rgbTable[PEXTW8(dVal, 5) + 65536];
				dest[6] = rgbTable[PEXTW8(dVal, 6) + 131072];
				dest[7] = rgbTable[PEXTW8(dVal, 7) + 196608];

				src += 8;
				dest += 8;
			}

			src += sAdd;
			dest += dAdd;
		}
	}
}
