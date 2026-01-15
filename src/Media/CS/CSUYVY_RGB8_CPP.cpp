#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSUYVY_RGB8.h"

extern "C" void CSUYVY_RGB8_do_uyvy2rgb(UInt8 *src, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, Int64 *y2r, UInt8 *rgbGammaCorr)
{
	UInt8 *yuv2rgb = (UInt8*)y2r;
	IntOS wsize = (width >> 1) - 2;
	IntOS dAdd = dbpl - (width << 2);
	IntOS i;
	Int32 ca;
	Int32 cr;
	Int32 cg;
	Int32 cb;
	Int32 aval;
	Int32 rval;
	Int32 gval;
	Int32 bval;
	Int32 aval2;
	Int32 rval2;
	Int32 gval2;
	Int32 bval2;
	while (height-- > 0)
	{
		cb = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 0] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 0];
		cg = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 2] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 2];
		cr = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 4] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 4];
		ca = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 6] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 6];
		bval = *(Int16*)&yuv2rgb[(src[1] << 3) + 0] + cb;
		gval = *(Int16*)&yuv2rgb[(src[1] << 3) + 2] + cg;
		rval = *(Int16*)&yuv2rgb[(src[1] << 3) + 4] + cr;
		aval = *(Int16*)&yuv2rgb[(src[1] << 3) + 6] + ca;
		if (bval >= 32768)
			bval = 32767;
		else if (bval < -32768)
			bval = -32768;
		if (gval >= 32768)
			gval = 32767;
		else if (gval < -32768)
			gval = -32768;
		if (rval >= 32768)
			rval = 32767;
		else if (rval < -32768)
			rval = -32768;
		if (aval >= 32768)
			aval = 32767;
		else if (aval < -32768)
			aval = -32768;
		cb = cb >> 1;
		cg = cg >> 1;
		cr = cr >> 1;
		ca = ca >> 1;
		dest[0] = rgbGammaCorr[(bval & 65535) + 131072];
		dest[1] = rgbGammaCorr[(gval & 65535) + 65536];
		dest[2] = rgbGammaCorr[rval & 65535];
		dest[3] = 0xff;
		bval = *(Int16*)&yuv2rgb[(src[3] << 3) + 0] + cb;
		gval = *(Int16*)&yuv2rgb[(src[3] << 3) + 2] + cg;
		rval = *(Int16*)&yuv2rgb[(src[3] << 3) + 4] + cr;
		aval = *(Int16*)&yuv2rgb[(src[3] << 3) + 6] + ca;
		src += 4;
		dest += 8;

		i = wsize;
		while (i-- > 0)
		{
			cb = *(Int16*)&yuv2rgb[(((UInt32)src[0]) << 3) + 2048 + 0] + (Int32)*(Int16*)&yuv2rgb[(((UInt32)src[2]) << 3) + 4096 + 0];
			cg = *(Int16*)&yuv2rgb[(((UInt32)src[0]) << 3) + 2048 + 2] + (Int32)*(Int16*)&yuv2rgb[(((UInt32)src[2]) << 3) + 4096 + 2];
			cr = *(Int16*)&yuv2rgb[(((UInt32)src[0]) << 3) + 2048 + 4] + (Int32)*(Int16*)&yuv2rgb[(((UInt32)src[2]) << 3) + 4096 + 4];
			ca = *(Int16*)&yuv2rgb[(((UInt32)src[0]) << 3) + 2048 + 6] + (Int32)*(Int16*)&yuv2rgb[(((UInt32)src[2]) << 3) + 4096 + 6];
			bval2 = cb + *(Int16*)&yuv2rgb[(((UInt32)src[1]) * 8) + 0];
			gval2 = cg + *(Int16*)&yuv2rgb[(((UInt32)src[1]) * 8) + 2];
			rval2 = cr + *(Int16*)&yuv2rgb[(((UInt32)src[1]) * 8) + 4];
			aval2 = ca + *(Int16*)&yuv2rgb[(((UInt32)src[1]) * 8) + 6];
			if (bval2 >= 32768)
				bval2 = 32767;
			else if (bval2 < -32768)
				bval2 = -32768;
			if (gval2 >= 32768)
				gval2 = 32767;
			else if (gval2 < -32768)
				gval2 = -32768;
			if (rval2 >= 32768)
				rval2 = 32767;
			else if (rval2 < -32768)
				rval2 = -32768;
			if (aval2 >= 32768)
				aval2 = 32767;
			else if (aval2 < -32768)
				aval2 = -32768;
			cb = cb >> 1;
			cg = cg >> 1;
			cr = cr >> 1;
			ca = ca >> 1;
			dest[0] = rgbGammaCorr[(bval2 & 65535) + 131072];
			dest[1] = rgbGammaCorr[(gval2 & 65535) + 65536];
			dest[2] = rgbGammaCorr[rval2 & 65535];
			dest[3] = 0xff;
			bval += cb;
			gval += cg;
			rval += cr;
			aval += ca;
			if (bval >= 32768)
				bval = 32767;
			else if (bval < -32768)
				bval = -32768;
			if (gval >= 32768)
				gval = 32767;
			else if (gval < -32768)
				gval = -32768;
			if (rval >= 32768)
				rval = 32767;
			else if (rval < -32768)
				rval = -32768;
			if (aval >= 32768)
				aval = 32767;
			else if (aval < -32768)
				aval = -32768;
			dest[-4] = rgbGammaCorr[(bval & 65535) + 131072];
			dest[-3] = rgbGammaCorr[(gval & 65535) + 65536];
			dest[-2] = rgbGammaCorr[rval & 65535];
			dest[-1] = 0xff;
			bval = cb + *(Int16*)&yuv2rgb[(((UInt32)src[3]) << 3) + 0];
			gval = cg + *(Int16*)&yuv2rgb[(((UInt32)src[3]) << 3) + 2];
			rval = cr + *(Int16*)&yuv2rgb[(((UInt32)src[3]) << 3) + 4];
			aval = ca + *(Int16*)&yuv2rgb[(((UInt32)src[3]) << 3) + 6];
			src += 4;
			dest += 8; 
		}
		cb = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 0] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 0];
		cg = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 2] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 2];
		cr = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 4] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 4];
		ca = *(Int16*)&yuv2rgb[(src[0] << 3) + 2048 + 6] + (Int32)*(Int16*)&yuv2rgb[(src[2] << 3) + 4096 + 6];
		bval2 = *(Int16*)&yuv2rgb[(src[1] << 3) + 0] + cb;
		gval2 = *(Int16*)&yuv2rgb[(src[1] << 3) + 2] + cg;
		rval2 = *(Int16*)&yuv2rgb[(src[1] << 3) + 4] + cr;
		aval2 = *(Int16*)&yuv2rgb[(src[1] << 3) + 6] + ca;
		if (bval2 >= 32768)
			bval2 = 32767;
		else if (bval2 < -32768)
			bval2 = -32768;
		if (gval2 >= 32768)
			gval2 = 32767;
		else if (gval2 < -32768)
			gval2 = -32768;
		if (rval2 >= 32768)
			rval2 = 32767;
		else if (rval2 < -32768)
			rval2 = -32768;
		if (aval2 >= 32768)
			aval2 = 32767;
		else if (aval2 < -32768)
			aval2 = -32768;
		dest[0] = rgbGammaCorr[(bval2 & 65535) + 131072];
		dest[1] = rgbGammaCorr[(gval2 & 65535) + 65536];
		dest[2] = rgbGammaCorr[rval2 & 65535];
		dest[3] = 0xff;
		bval2 = *(Int16*)&yuv2rgb[(src[3] << 3) + 0] + cb;
		gval2 = *(Int16*)&yuv2rgb[(src[3] << 3) + 2] + cg;
		rval2 = *(Int16*)&yuv2rgb[(src[3] << 3) + 4] + cr;
		aval2 = *(Int16*)&yuv2rgb[(src[3] << 3) + 6] + ca;
		if (bval2 >= 32768)
			bval2 = 32767;
		else if (bval2 < -32768)
			bval2 = -32768;
		if (gval2 >= 32768)
			gval2 = 32767;
		else if (gval2 < -32768)
			gval2 = -32768;
		if (rval2 >= 32768)
			rval2 = 32767;
		else if (rval2 < -32768)
			rval2 = -32768;
		if (aval2 >= 32768)
			aval2 = 32767;
		else if (aval2 < -32768)
			aval2 = -32768;
		dest[4] = rgbGammaCorr[(bval2 & 65535) + 131072];
		dest[5] = rgbGammaCorr[(gval2 & 65535) + 65536];
		dest[6] = rgbGammaCorr[rval2 & 65535];
		dest[7] = 0xff;
		cb = cb >> 1;
		cg = cg >> 1;
		cr = cr >> 1;
		ca = ca >> 1;
		bval += cb;
		gval += cg;
		rval += cr;
		aval += ca;
		if (bval >= 32768)
			bval = 32767;
		else if (bval < -32768)
			bval = -32768;
		if (gval >= 32768)
			gval = 32767;
		else if (gval < -32768)
			gval = -32768;
		if (rval >= 32768)
			rval = 32767;
		else if (rval < -32768)
			rval = -32768;
		if (aval >= 32768)
			aval = 32767;
		else if (aval < -32768)
			aval = -32768;
		dest[-4] = rgbGammaCorr[(bval & 65535) + 131072];
		dest[-3] = rgbGammaCorr[(gval & 65535) + 65536];
		dest[-2] = rgbGammaCorr[rval & 65535];
		dest[-1] = 0xff;

		src += 4;
		dest += 8 + dAdd;
	}
}

