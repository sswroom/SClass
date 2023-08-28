#include "Stdafx.h"
#include "MyMemory.h"
#include "SIMD.h"

extern "C" void CSYUY2_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, OSInt width, OSInt height, OSInt dbpl, Int64 *yuv2rgbi64, Int64 *rgbGammaCorri64)
{
	OSInt i;
	Int16x4 i16Val1;
	Int16x4 i16Val2;
	Int16x4 i16Val3;
	Int16x4 i16Val4;
	UInt8 *yuv2rgb = (UInt8*)yuv2rgbi64;
	UInt8 *rgbGammaCorr = (UInt8*)rgbGammaCorri64;
	dbpl = dbpl - width * 8;
	width = (width >> 1) - 2;

	while (height-- > 0)
	{
		i = width;

		i16Val3 = PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * src[1]]), PLoadInt16x4(&yuv2rgb[4096 + 8 * src[3]]));
		i16Val1 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[0]]), i16Val3);
		i16Val2 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[2]]), PSARW4(i16Val3, 1));

		i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val1, 0)]);
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val1, 1)]));
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val1, 2)]));
		PStoreInt16x4(dest, i16Val3);
		dest += 16;
		src += 4;

		while (i-- > 0)
		{
			i16Val3 = PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * src[1]]), PLoadInt16x4(&yuv2rgb[4096 + 8 * src[3]]));
			i16Val1 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[0]]), i16Val3);
			i16Val2 = PSADDW4(i16Val2, PSARW4(i16Val3, 1));
			i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val1, 0)]);
			i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val1, 1)]));
			i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val1, 2)]));
			PStoreInt16x4(dest, i16Val4);

			i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val2, 0)]);
			i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val2, 1)]));
			i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val2, 2)]));
			PStoreInt16x4(&dest[-8], i16Val4);

			i16Val2 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[2]]), PSARW4(i16Val3, 1));

			src += 4;
			dest += 16;
		}

		i16Val3 = PSADDW4(PLoadInt16x4(&yuv2rgb[2048 + 8 * src[1]]), PLoadInt16x4(&yuv2rgb[4096 + 8 * src[3]]));
		i16Val1 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[0]]), i16Val3);
		i16Val2 = PSADDW4(i16Val2, PSARW4(i16Val3, 1));
		i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val1, 0)]);
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val1, 1)]));
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val1, 2)]));
		PStoreInt16x4(dest, i16Val4);

		i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val2, 0)]);
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val2, 1)]));
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val2, 2)]));
		PStoreInt16x4(&dest[-8], i16Val4);

		i16Val2 = PSADDW4(PLoadInt16x4(&yuv2rgb[8 * src[2]]), i16Val3);
		i16Val4 = PLoadInt16x4(&rgbGammaCorr[1048576 + 8 * (UInt16)PEXTW4(i16Val2, 0)]);
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[524288 + 8 * (UInt16)PEXTW4(i16Val2, 1)]));
		i16Val4 = PSADDW4(i16Val4, PLoadInt16x4(&rgbGammaCorr[0 + 8 * (UInt16)PEXTW4(i16Val2, 2)]));
		PStoreInt16x4(&dest[8], i16Val4);

		src += 4;
		dest += 16;
		dest += dbpl;
	}
}