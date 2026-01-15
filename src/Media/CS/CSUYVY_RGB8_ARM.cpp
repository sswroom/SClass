#include "stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUY2_RGB8.h"
#include <arm_neon.h>

extern "C" void CSYUY2_RGB8_do_yuy2rgb(UInt8 *src, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *yuv2rgb, UInt8 *rgbGammaCorr)
{
	IntOS wsize = (width >> 1) - 2;
	IntOS dAdd = dbpl - (width << 2);
	IntOS i;
	int16x4_t cval;
	int16x4_t rgbval;
	int16x4_t rgbval2;
	while (height-- > 0)
	{
		cval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[0] + 256], *(int16x4_t*)&yuv2rgb[src[2] + 512]);
		rgbval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[1]], cval);
		cval = vshr_n_s16(cval, 1);
		dest[0] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 0)) + 131072];
		dest[1] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 1)) + 65536];
		dest[2] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 2))];
		dest[3] = 0xff;
		rgbval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[3]], cval);
		src += 4;
		dest += 8;

		i = wsize;
		while (i-- > 0)
		{
			cval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[0] + 256], *(int16x4_t*)&yuv2rgb[src[2] + 512]);
			rgbval2 = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[1]], cval);
			cval = vshr_n_s16(cval, 1);
			dest[0] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 0)) + 131072];
			dest[1] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 1)) + 65536];
			dest[2] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 2))];
			dest[3] = 0xff;
			rgbval = vqadd_s16(rgbval, cval);
			dest[-4] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 0)) + 131072];
			dest[-3] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 1)) + 65536];
			dest[-2] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 2))];
			dest[-1] = 0xff;
			rgbval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[3]], cval);
			src += 4;
			dest += 8; 
		}
		cval = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[0] + 256], *(int16x4_t*)&yuv2rgb[src[2] + 512]);
		rgbval2 = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[1]], cval);
		dest[0] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 0)) + 131072];
		dest[1] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 1)) + 65536];
		dest[2] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 2))];
		dest[3] = 0xff;
		rgbval2 = vqadd_s16(*(int16x4_t*)&yuv2rgb[src[3]], cval);
		dest[4] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 0)) + 131072];
		dest[5] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 1)) + 65536];
		dest[6] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval2, 2))];
		dest[7] = 0xff;
		cval = vshr_n_s16(cval, 1);
		rgbval = vqadd_s16(rgbval, cval);
		dest[-4] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 0)) + 131072];
		dest[-3] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 1)) + 65536];
		dest[-2] = rgbGammaCorr[((UInt16)vget_lane_s16(rgbval, 2))];
		dest[-1] = 0xff;

		src += 4;
		dest += 8 + dAdd;
	}
}

