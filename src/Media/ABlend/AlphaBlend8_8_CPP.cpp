#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Media/ABlend/AlphaBlend8_8.h"
#include "Sync/ThreadUtil.h"


extern "C" void AlphaBlend8_8_DoBlend(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;

	UInt16x4 cval;
	UInt16x4 cAdd = PUInt16x4SetA(16);
	UInt16x4 daVal;
	UInt8x4 zeroU8 = PUInt8x4Clear();
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			daVal = PUInt16x4SetA((UInt16)(255 - src[3]));
			daVal = PINSUW4(daVal, 3, 255);
			cval = PSADDUW4(PMULULW4(PUNPCKUBW4(PLoadUInt8x4(src), zeroU8), PUInt16x4SetA(src[3])), PMULULW4(PUNPCKUBW4(PLoadUInt8x4(dest), zeroU8), daVal));
			PStoreUInt8x4(dest, PSHRADDWB4(PSADDUW4(PSHRW4(cval, 8), cval), cAdd, 8));

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}

extern "C" void AlphaBlend8_8_DoBlendPA(UInt8 *dest, OSInt dbpl, const UInt8 *src, OSInt sbpl, OSInt width, OSInt height)
{
	OSInt sAdd = sbpl - width * 4;
	OSInt dAdd = dbpl - width * 4;
	OSInt i;

	UInt16x4 cval;
	UInt16x4 cAdd = PUInt16x4SetA(16);
	UInt16x4 cMul = PUInt16x4SetA(255);
	UInt16x4 daVal;
	UInt8x4 zeroU8 = PUInt8x4Clear();
	while (height-- > 0)
	{
		i = width;
		while (i-- > 0)
		{
			daVal = PUInt16x4SetA((UInt16)(255 - src[3]));
			daVal = PINSUW4(daVal, 3, 255);
			cval = PADDUW4(PMULULW4(PUNPCKUBW4(PLoadUInt8x4(src), zeroU8), cMul), PMULULW4(PUNPCKUBW4(PLoadUInt8x4(dest), zeroU8), daVal));
			PStoreUInt8x4(dest, PSHRADDWB4(PSADDUW4(PSHRW4(cval, 8), cval), cAdd, 8));

			src += 4;
			dest += 4;
		}

		src += sAdd;
		dest += dAdd;
	}
}