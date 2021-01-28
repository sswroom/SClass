#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"

extern "C" void ImageUtil_SwapRGB(UInt8 *imgPtr, OSInt pixelCnt, OSInt bpp)
{
	if (bpp == 48)
	{
		UInt16 v;
		while (pixelCnt-- > 0)
		{
			v = *(UInt16*)&imgPtr[0];
			*(UInt16*)&imgPtr[0] = *(UInt16*)&imgPtr[4];
			*(UInt16*)&imgPtr[4] = v;
			imgPtr += 6;
		}
	}
	else if (bpp == 64)
	{
		UInt16 v;
		while (pixelCnt-- > 0)
		{
			v = *(UInt16*)&imgPtr[0];
			*(UInt16*)&imgPtr[0] = *(UInt16*)&imgPtr[4];
			*(UInt16*)&imgPtr[4] = v;
			imgPtr += 8;
		}
	}
	else if (bpp == 24)
	{
		UInt8 v;
		while (pixelCnt-- > 0)
		{
			v = imgPtr[0];
			imgPtr[0] = imgPtr[2];
			imgPtr[2] = v;
			imgPtr += 3;
		}
	}
	else if (bpp == 32)
	{
		UInt8 v;
		while (pixelCnt-- > 0)
		{
			v = imgPtr[0];
			imgPtr[0] = imgPtr[2];
			imgPtr[2] = v;
			imgPtr += 4;
		}
	}
}

extern "C" void ImageUtil_ColorReplace32(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col)
{
	OSInt pxCnt = w * h;
	while (pxCnt-- > 0)
	{
		if (pixelPtr[0] != 0)
		{
			*(Int32*)pixelPtr = col;
		}
		pixelPtr += 4;
	}
}


extern "C" void ImageUtil_ColorReplace32A(UInt8 *pixelPtr, OSInt w, OSInt h, Int32 col)
{
	OSInt pxCnt = w * h;
	while (pxCnt-- > 0)
	{
		if (pixelPtr[0] != 0)
		{
			*(Int32*)pixelPtr = col;
		}
		pixelPtr += 4;
	}
}

extern "C" void ImageUtil_ColorFill32(UInt8 *pixelPtr, OSInt pixelCnt, Int32 color)
{
	while (pixelCnt-- > 0)
	{
		*(Int32*)pixelPtr = color;
		pixelPtr += 4;
	}
}

extern "C" void ImageUtil_ImageColorReplace32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col)
{
	OSInt i;
	const UInt8 *sPtr;
	UInt8 *dPtr;
	while (h-- > 0)
	{
		sPtr = srcPtr;
		dPtr = destPtr;
		i = w;
		while (i-- > 0)
		{
			if (*(Int32*)sPtr)
			{
				*(Int32*)dPtr = col;
			}
			sPtr += 4;
			dPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ImageMaskABlend32(const UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col)
{
	UInt32 a = (((UInt32)col) >> 24);
	UInt32 cMul = 255 - a;
	UInt32 aAdd;
	UInt32 rAdd;
	UInt32 gAdd;
	UInt32 bAdd;
	UInt32 v;
	OSInt i;
	cMul = cMul | (cMul << 8);
	aAdd = a | (a << 8);
	rAdd = (col >> 16) & 0xff;
	gAdd = (col >> 8) & 0xff;
	bAdd = col & 0xff;
	rAdd = ((rAdd | (rAdd << 8)) * aAdd) >> 16;
	gAdd = ((gAdd | (gAdd << 8)) * aAdd) >> 16;
	bAdd = ((bAdd | (bAdd << 8)) * aAdd) >> 16;
	const UInt8 *sPtr;
	UInt8 *dPtr;
	while (h-- > 0)
	{
		sPtr = maskPtr;
		dPtr = destPtr;
		i = w;
		while (i-- > 0)
		{
			if (*(UInt32*)sPtr)
			{
				v = dPtr[0];
				v = ((((v | (v << 8)) * cMul) >> 16) + bAdd) >> 8;
				if (v > 255)
					dPtr[0] = 255;
				else
					dPtr[0] = (UInt8)v;
				v = dPtr[1];
				v = ((((v | (v << 8)) * cMul) >> 16) + gAdd) >> 8;
				if (v > 255)
					dPtr[1] = 255;
				else
					dPtr[1] = (UInt8)v;
				v = dPtr[2];
				v = ((((v | (v << 8)) * cMul) >> 16) + rAdd) >> 8;
				if (v > 255)
					dPtr[2] = 255;
				else
					dPtr[2] = (UInt8)v;
				v = dPtr[3];
				v = ((((v | (v << 8)) * cMul) >> 16) + aAdd) >> 8;
				if (v > 255)
					dPtr[3] = 255;
				else
					dPtr[3] = (UInt8)v;
			}
			sPtr += 4;
			dPtr += 4;
		}
		maskPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ImageMask2ABlend32(const UInt8 *maskPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, Int32 col1, Int32 col2)
{
	UInt32 a1 = (((UInt32)col1) >> 24);
	UInt32 a2 = (((UInt32)col2) >> 24);
	UInt32 cMul1 = 255 - a1;
	UInt32 cMul2 = 255 - a2;
	UInt32 aAdd1;
	UInt32 rAdd1;
	UInt32 gAdd1;
	UInt32 bAdd1;
	UInt32 aAdd2;
	UInt32 rAdd2;
	UInt32 gAdd2;
	UInt32 bAdd2;
	UInt32 v;
	OSInt i;
	cMul1 = cMul1 | (cMul1 << 8);
	cMul2 = cMul2 | (cMul2 << 8);
	aAdd1 = a1 | (a1 << 8);
	aAdd2 = a2 | (a2 << 8);
	rAdd1 = (col1 >> 16) & 0xff;
	gAdd1 = (col1 >> 8) & 0xff;
	bAdd1 = col1 & 0xff;
	rAdd2 = (col2 >> 16) & 0xff;
	gAdd2 = (col2 >> 8) & 0xff;
	bAdd2 = col2 & 0xff;
	rAdd1 = ((rAdd1 | (rAdd1 << 8)) * aAdd1) >> 16;
	gAdd1 = ((gAdd1 | (gAdd1 << 8)) * aAdd1) >> 16;
	bAdd1 = ((bAdd1 | (bAdd1 << 8)) * aAdd1) >> 16;
	rAdd2 = ((rAdd2 | (rAdd2 << 8)) * aAdd2) >> 16;
	gAdd2 = ((gAdd2 | (gAdd2 << 8)) * aAdd2) >> 16;
	bAdd2 = ((bAdd2 | (bAdd2 << 8)) * aAdd2) >> 16;
	const UInt8 *sPtr;
	UInt8 *dPtr;
	while (h-- > 0)
	{
		sPtr = maskPtr;
		dPtr = destPtr;
		i = w;
		while (i-- > 0)
		{
			v = *(UInt32*)sPtr;
			if (v == 0)
			{
				v = dPtr[0];
				v = ((((v | (v << 8)) * cMul1) >> 16) + bAdd1) >> 8;
				if (v > 255)
					dPtr[0] = 255;
				else
					dPtr[0] = (UInt8)v;
				v = dPtr[1];
				v = ((((v | (v << 8)) * cMul1) >> 16) + gAdd1) >> 8;
				if (v > 255)
					dPtr[1] = 255;
				else
					dPtr[1] = (UInt8)v;
				v = dPtr[2];
				v = ((((v | (v << 8)) * cMul1) >> 16) + rAdd1) >> 8;
				if (v > 255)
					dPtr[2] = 255;
				else
					dPtr[2] = (UInt8)v;
				v = dPtr[3];
				v = ((((v | (v << 8)) * cMul1) >> 16) + aAdd1) >> 8;
				if (v > 255)
					dPtr[3] = 255;
				else
					dPtr[3] = (UInt8)v;
			}
			else if (v != 0xff000000)
			{
				v = dPtr[0];
				v = ((((v | (v << 8)) * cMul2) >> 16) + bAdd2) >> 8;
				if (v > 255)
					dPtr[0] = 255;
				else
					dPtr[0] = (UInt8)v;
				v = dPtr[1];
				v = ((((v | (v << 8)) * cMul2) >> 16) + gAdd2) >> 8;
				if (v > 255)
					dPtr[1] = 255;
				else
					dPtr[1] = (UInt8)v;
				v = dPtr[2];
				v = ((((v | (v << 8)) * cMul2) >> 16) + rAdd2) >> 8;
				if (v > 255)
					dPtr[2] = 255;
				else
					dPtr[2] = (UInt8)v;
				v = dPtr[3];
				v = ((((v | (v << 8)) * cMul2) >> 16) + aAdd2) >> 8;
				if (v > 255)
					dPtr[3] = 255;
				else
					dPtr[3] = (UInt8)v;
			}
			sPtr += 4;
			dPtr += 4;
		}
		maskPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ImageColorBuffer32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, OSInt buffSize)
{
	UInt8 *tmpPtr;
	UInt8 *tmpPtr1;
	UInt8 *tmpPtr2;
	OSInt i;
	OSInt j;
	OSInt k;
	while (h-- > 0)
	{
		tmpPtr = pixelPtr;
		i = w;
		while (i-- > 0)
		{
			if (0x808080 & *(UInt32*)tmpPtr)
			{
				j = 0;
				tmpPtr1 = tmpPtr;
				tmpPtr2 = tmpPtr;
				while (j < buffSize)
				{
					k = buffSize - j;
					while (k >= 0)
					{
						if (*(UInt32*)&tmpPtr1[k * 4] == 0)
							*(UInt32*)&tmpPtr1[k * 4] = 0x7f7f7f;
						if (*(UInt32*)&tmpPtr2[k * 4] == 0)
							*(UInt32*)&tmpPtr2[k * 4] = 0x7f7f7f;
						if (*(UInt32*)&tmpPtr1[-k * 4] == 0)
							*(UInt32*)&tmpPtr1[-k * 4] = 0x7f7f7f;
						if (*(UInt32*)&tmpPtr2[-k * 4] == 0)
							*(UInt32*)&tmpPtr2[-k * 4] = 0x7f7f7f;
						k--;
					}
					j++;
					tmpPtr1 -= bpl;
					tmpPtr2 += bpl;
				}
			}
			tmpPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_ImageColorFill32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
{
	UInt8 *tmpPtr;
	OSInt i;
	while (h-- > 0)
	{
		i = w;
		tmpPtr = pixelPtr;
		while (i-- > 0)
		{
			*(Int32*)tmpPtr = col;
			tmpPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_ImageColorBlend32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, Int32 col)
{
	UInt32 a = (col >> 24);
	UInt32 cMul = 255 - a;
	UInt32 aAdd;
	UInt32 rAdd;
	UInt32 gAdd;
	UInt32 bAdd;
	cMul = cMul | (cMul << 8);
	aAdd = a | (a << 8);
	rAdd = (col >> 16) & 0xff;
	gAdd = (col >> 8) & 0xff;
	bAdd = col & 0xff;
	rAdd = ((rAdd | (rAdd << 8)) * aAdd) >> 16;
	gAdd = ((gAdd | (gAdd << 8)) * aAdd) >> 16;
	bAdd = ((bAdd | (bAdd << 8)) * aAdd) >> 16;
	UInt8 *tmpPtr;
	UInt32 v;
	OSInt i;
	while (h-- > 0)
	{
		tmpPtr = pixelPtr;
		i = w;
		while (i-- > 0)
		{
			v = tmpPtr[0];
			v = ((((v | (v << 8)) * cMul) >> 16) + bAdd) >> 8;
			if (v > 255)
				tmpPtr[0] = 255;
			else
				tmpPtr[0] = (UInt8)v;
			v = tmpPtr[1];
			v = ((((v | (v << 8)) * cMul) >> 16) + gAdd) >> 8;
			if (v > 255)
				tmpPtr[1] = 255;
			else
				tmpPtr[1] = (UInt8)v;
			v = tmpPtr[2];
			v = ((((v | (v << 8)) * cMul) >> 16) + rAdd) >> 8;
			if (v > 255)
				tmpPtr[2] = 255;
			else
				tmpPtr[2] = (UInt8)v;
			v = tmpPtr[3];
			v = ((((v | (v << 8)) * cMul) >> 16) + aAdd) >> 8;
			if (v > 255)
				tmpPtr[3] = 255;
			else
				tmpPtr[3] = (UInt8)v;
			tmpPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_ImageFillAlpha32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt8 a)
{
	OSInt cnt;
	bpl -= w * 4;
	while (h-- > 0)
	{
		cnt = w;
		while (cnt-- > 0)
		{
			pixelPtr[3] = a;
			pixelPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_ImageAlphaMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 a)
{
	UInt8 *tmpPtr;
	OSInt cnt;
	while (h-- > 0)
	{
		tmpPtr = pixelPtr;
		cnt = w;
		while (cnt-- > 0)
		{
			tmpPtr[3] = (tmpPtr[3] * a) >> 16;
			tmpPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_ImageColorMul32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 c)
{
	UInt32 b;
	UInt32 g;
	UInt32 r;
	UInt32 a;
	b = c & 0xff;
	g = (c >> 8) & 0xff;
	r = (c >> 16) & 0xff;
	a = (c >> 24) & 0xff;
	b = b | (b << 8);
	g = g | (g << 8);
	r = r | (r << 8);
	a = a | (a << 8);

	UInt8 *tmpPtr;
	OSInt cnt;
	while (h-- > 0)
	{
		tmpPtr = pixelPtr;
		cnt = w;
		while (cnt-- > 0)
		{
			tmpPtr[0] = (tmpPtr[0] * b) >> 16;
			tmpPtr[1] = (tmpPtr[1] * g) >> 16;
			tmpPtr[2] = (tmpPtr[2] * r) >> 16;
			tmpPtr[3] = (tmpPtr[3] * a) >> 16;
			tmpPtr += 4;
		}
		pixelPtr += bpl;
	}
}

extern "C" void ImageUtil_DrawRectNA32(UInt8 *pixelPtr, OSInt w, OSInt h, OSInt bpl, UInt32 col)
{
	h -= 2;
	if (h < 0)
		return;
	OSInt bpl2 = bpl - w * 4;
	OSInt cnt;
	cnt = w;
	while (cnt-- > 0)
	{
		WriteUInt32(pixelPtr, col);
		pixelPtr += 4;
	}
	pixelPtr += bpl2;
	while (h-- > 0)
	{
		WriteUInt32(pixelPtr, col);
		WriteUInt32(&pixelPtr[w * 4 - 4], col);
		pixelPtr += bpl;
	}
	cnt = w;
	while (cnt-- > 0)
	{
		WriteUInt32(pixelPtr, col);
		pixelPtr += 4;
	}
	pixelPtr += bpl2;
}

extern "C" void ImageUtil_ConvP1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	dbpl -= w << 2;
	sbpl -= w >> 3;
	while (h-- > 0)
	{
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			*(Int32*)&destPtr[0] = *(Int32*)&pal[((v >> 7) & 1) << 2];
			*(Int32*)&destPtr[4] = *(Int32*)&pal[((v >> 6) & 1) << 2];
			*(Int32*)&destPtr[8] = *(Int32*)&pal[((v >> 5) & 1) << 2];
			*(Int32*)&destPtr[12] = *(Int32*)&pal[((v >> 4) & 1) << 2];
			*(Int32*)&destPtr[16] = *(Int32*)&pal[((v >> 3) & 1) << 2];
			*(Int32*)&destPtr[20] = *(Int32*)&pal[((v >> 2) & 1) << 2];
			*(Int32*)&destPtr[24] = *(Int32*)&pal[((v >> 1) & 1) << 2];
			*(Int32*)&destPtr[28] = *(Int32*)&pal[(v & 1) << 2];
			destPtr += 32;
		}

		i = w & 7;
		if (i > 0)
		{
			v = *srcPtr;
			while (i-- > 0)
			{
				*(Int32*)&destPtr[0] = *(Int32*)&pal[((v >> 7) & 1) << 2];
				destPtr += 4;
				v = v << 1;
			}
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP2_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	dbpl -= w << 2;
	sbpl -= w >> 2;
	while (h-- > 0)
	{
		i = w >> 2;
		while (i-- > 0)
		{
			v = *srcPtr++;
			*(Int32*)&destPtr[0] = *(Int32*)&pal[((v >> 6) & 3) << 2];
			*(Int32*)&destPtr[4] = *(Int32*)&pal[((v >> 4) & 3) << 2];
			*(Int32*)&destPtr[8] = *(Int32*)&pal[((v >> 2) & 3) << 2];
			*(Int32*)&destPtr[12] = *(Int32*)&pal[(v & 3) << 2];
			destPtr += 16;
		}

		i = w & 3;
		if (i > 0)
		{
			v = *srcPtr;
			while (i-- > 0)
			{
				*(Int32*)&destPtr[0] = *(Int32*)&pal[((v >> 6) & 3) << 2];
				destPtr += 4;
				v = v << 2;

			}
		}

		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP4_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	if (w & 1)
	{
		dbpl -= w << 2;
		w = w >> 1;
		sbpl -= w;
		while (h-- > 0)
		{
			i = w;
			while (i-- > 0)
			{
				*(Int32*)&destPtr[0] = *(Int32*)&pal[(*srcPtr >> 4) << 2];
				*(Int32*)&destPtr[4] = *(Int32*)&pal[(*srcPtr & 15) << 2];
				srcPtr += 1;
				destPtr += 8;
			}

			*(Int32*)&destPtr[0] = *(Int32*)&pal[(*srcPtr >> 4) << 2];
			destPtr += 4;

			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
	else
	{
		dbpl -= w << 2;
		w = w >> 1;
		sbpl -= w;
		while (h-- > 0)
		{
			i = w;
			while (i-- > 0)
			{
				*(Int32*)&destPtr[0] = *(Int32*)&pal[(*srcPtr >> 4) << 2];
				*(Int32*)&destPtr[4] = *(Int32*)&pal[(*srcPtr & 15) << 2];
				srcPtr += 1;
				destPtr += 8;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
}

extern "C" void ImageUtil_ConvP8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	sbpl -= w;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			*(Int32*)&destPtr[0] = *(Int32*)&pal[*srcPtr << 2];
			srcPtr += 1;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP1_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	UInt8 *dptr;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			*(Int32*)&dptr[0] = *(Int32*)&pal[((v >> 7) & 1) << 2];
			*(Int32*)&dptr[4] = *(Int32*)&pal[((v >> 6) & 1) << 2];
			*(Int32*)&dptr[8] = *(Int32*)&pal[((v >> 5) & 1) << 2];
			*(Int32*)&dptr[12] = *(Int32*)&pal[((v >> 4) & 1) << 2];
			*(Int32*)&dptr[16] = *(Int32*)&pal[((v >> 3) & 1) << 2];
			*(Int32*)&dptr[20] = *(Int32*)&pal[((v >> 2) & 1) << 2];
			*(Int32*)&dptr[24] = *(Int32*)&pal[((v >> 1) & 1) << 2];
			*(Int32*)&dptr[28] = *(Int32*)&pal[(v & 1) << 2];
			dptr += 32;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				*(Int32*)&dptr[0] = *(Int32*)&pal[((v >> 7) & 1) << 2];
				dptr += 4;
				v = v << 1;
			}
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[3] = -((v >> 7) & 1);
			dptr[7] = -((v >> 6) & 1);
			dptr[11] = -((v >> 5) & 1);
			dptr[15] = -((v >> 4) & 1);
			dptr[19] = -((v >> 3) & 1);
			dptr[23] = -((v >> 2) & 1);
			dptr[27] = -((v >> 1) & 1);
			dptr[31] = -(v & 1);
			dptr += 32;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[3] = -((v >> 7) & 1);
				dptr += 4;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP2_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	UInt8 *dptr;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 2;
		while (i-- > 0)
		{
			v = *srcPtr++;
			*(Int32*)&dptr[0] = *(Int32*)&pal[((v >> 6) & 3) << 2];
			*(Int32*)&dptr[4] = *(Int32*)&pal[((v >> 4) & 3) << 2];
			*(Int32*)&dptr[8] = *(Int32*)&pal[((v >> 2) & 3) << 2];
			*(Int32*)&dptr[12] = *(Int32*)&pal[(v & 3) << 2];
			dptr += 16;
		}
		if (w & 3)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 3;
			while (i-- > 0)
			{
				*(Int32*)&dptr[0] = *(Int32*)&pal[((v >> 6) & 3) << 2];
				dptr += 4;
				v = v << 2;
			}
		}


		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[3] = -((v >> 7) & 1);
			dptr[7] = -((v >> 6) & 1);
			dptr[11] = -((v >> 5) & 1);
			dptr[15] = -((v >> 4) & 1);
			dptr[19] = -((v >> 3) & 1);
			dptr[23] = -((v >> 2) & 1);
			dptr[27] = -((v >> 1) & 1);
			dptr[31] = -(v & 1);
			dptr += 32;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[3] = -((v >> 7) & 1);
				dptr += 4;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP4_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	UInt8 *dptr;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 1;
		while (i-- > 0)
		{
			*(Int32*)&dptr[0] = *(Int32*)&pal[(*srcPtr >> 4) << 2];
			*(Int32*)&dptr[4] = *(Int32*)&pal[(*srcPtr & 15) << 2];
			srcPtr += 1;
			dptr += 8;
		}
		if (w & 1)
		{
			*(Int32*)&dptr[0] = *(Int32*)&pal[(srcPtr[0] >> 4) << 2];
			dptr += 4;
			srcPtr++;
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[3] = -((v >> 7) & 1);
			dptr[7] = -((v >> 6) & 1);
			dptr[11] = -((v >> 5) & 1);
			dptr[15] = -((v >> 4) & 1);
			dptr[19] = -((v >> 3) & 1);
			dptr[23] = -((v >> 2) & 1);
			dptr[27] = -((v >> 1) & 1);
			dptr[31] = -(v & 1);
			dptr += 32;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[3] = -((v >> 7) & 1);
				dptr += 4;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP8_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	UInt8 v;
	UInt8 *dptr;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w;
		while (i-- > 0)
		{
			*(Int32*)&dptr[0] = *(Int32*)&pal[*srcPtr << 2];
			srcPtr += 1;
			dptr += 4;
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[3] = -((v >> 7) & 1);
			dptr[7] = -((v >> 6) & 1);
			dptr[11] = -((v >> 5) & 1);
			dptr[15] = -((v >> 4) & 1);
			dptr[19] = -((v >> 3) & 1);
			dptr[23] = -((v >> 2) & 1);
			dptr[27] = -((v >> 1) & 1);
			dptr[31] = -(v & 1);
			dptr += 32;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[3] = -((v >> 7) & 1);
				dptr += 4;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB5G5R5_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	UInt8 cv;
	UInt16 v;
	sbpl -= w << 1;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(srcPtr);
			cv = v & 0x1f;
			destPtr[0] = (cv << 3) | (cv >> 2);
			cv = (v >> 5) & 0x1f;
			destPtr[1] = (cv << 3) | (cv >> 2);
			cv = v >> 10;
			destPtr[2] = (cv << 3) | (cv >> 2);
			destPtr[3] = 0xff;
			srcPtr += 2;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB5G6R5_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	UInt8 cv;
	UInt16 v;
	sbpl -= w << 1;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(srcPtr);
			cv = v & 0x1f;
			destPtr[0] = (cv << 3) | (cv >> 2);
			cv = (v >> 5) & 0x3f;
			destPtr[1] = (cv << 2) | (cv >> 4);
			cv = v >> 11;
			destPtr[2] = (cv << 3) | (cv >> 2);
			destPtr[3] = 0xff;
			srcPtr += 2;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB8G8R8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 3;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[0];
			destPtr[1] = srcPtr[1];
			destPtr[2] = srcPtr[2];
			destPtr[3] = 0xff;
			srcPtr += 3;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvR8G8B8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 3;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[2];
			destPtr[1] = srcPtr[1];
			destPtr[2] = srcPtr[0];
			destPtr[3] = 0xff;
			srcPtr += 3;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvR8G8B8A8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 4;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[2];
			destPtr[1] = srcPtr[1];
			destPtr[2] = srcPtr[0];
			destPtr[3] = srcPtr[3];
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvR8G8B8N8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 4;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[2];
			destPtr[1] = srcPtr[1];
			destPtr[2] = srcPtr[0];
			destPtr[3] = 0xff;
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvARGB48_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 6;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[1];
			destPtr[1] = srcPtr[3];
			destPtr[2] = srcPtr[5];
			destPtr[3] = 0xff;
			srcPtr += 6;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvARGB64_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w << 3;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[1];
			destPtr[1] = srcPtr[3];
			destPtr[2] = srcPtr[5];
			destPtr[3] = srcPtr[7];
			srcPtr += 8;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvA2B10G10R10_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt16 r;
	UInt16 g;
	UInt16 b;
	UInt16 a;
	UInt32 v;
	OSInt i;
	sbpl -= w << 2;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt32(srcPtr);
			r = v & 0x3ff;
			g = (v >> 10) & 0x3ff;
			b = (v >> 20) & 0x3ff;
			a = (v >> 30) & 3;
			a = (a << 2) | a;
			a = (a << 4) | a;
			destPtr[0] = (UInt8)(b >> 2);
			destPtr[1] = (UInt8)(g >> 2);
			destPtr[2] = (UInt8)(r >> 2);
			destPtr[3] = (UInt8)a;
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFB32G32R32A32_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w << 4;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 255;
			if (v > 255)
			{
				destPtr[0] = 255;
			}
			else if (v < 0)
			{
				destPtr[0] = 0;
			}
			else
			{
				destPtr[0] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[4]) * 255;
			if (v > 255)
			{
				destPtr[1] = 255;
			}
			else if (v < 0)
			{
				destPtr[1] = 0;
			}
			else
			{
				destPtr[1] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[8]) * 255;
			if (v > 255)
			{
				destPtr[2] = 255;
			}
			else if (v < 0)
			{
				destPtr[2] = 0;
			}
			else
			{
				destPtr[2] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[12]) * 65535;
			if (v > 255)
			{
				destPtr[3] = 255;
			}
			else if (v < 0)
			{
				destPtr[3] = 0;
			}
			else
			{
				destPtr[3] = (UInt8)Math::Double2Int32(v);
			}
			srcPtr += 16;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFB32G32R32_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w *12;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 255;
			if (v > 255)
			{
				destPtr[0] = 255;
			}
			else if (v < 0)
			{
				destPtr[0] = 0;
			}
			else
			{
				destPtr[0] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[4]) * 255;
			if (v > 255)
			{
				destPtr[1] = 255;
			}
			else if (v < 0)
			{
				destPtr[1] = 0;
			}
			else
			{
				destPtr[1] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[8]) * 255;
			if (v > 255)
			{
				destPtr[2] = 255;
			}
			else if (v < 0)
			{
				destPtr[2] = 0;
			}
			else
			{
				destPtr[2] = (UInt8)Math::Double2Int32(v);
			}
			destPtr[3] = 255;
			srcPtr += 12;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFW32A32_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w << 3;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 255;
			if (v > 255)
			{
				destPtr[0] = 255;
				destPtr[1] = 255;
				destPtr[2] = 255;
			}
			else if (v < 0)
			{
				destPtr[0] = 0;
				destPtr[1] = 0;
				destPtr[2] = 0;
			}
			else
			{
				destPtr[0] = (UInt8)Math::Double2Int32(v);
				destPtr[1] = (UInt8)Math::Double2Int32(v);
				destPtr[2] = (UInt8)Math::Double2Int32(v);
			}
			v = ReadFloat(&srcPtr[4]) * 65535;
			if (v > 255)
			{
				destPtr[3] = 255;
			}
			else if (v < 0)
			{
				destPtr[3] = 0;
			}
			else
			{
				destPtr[3] = (UInt8)Math::Double2Int32(v);
			}
			srcPtr += 8;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFW32_32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w << 4;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 255;
			if (v > 255)
			{
				destPtr[0] = 255;
				destPtr[1] = 255;
				destPtr[2] = 255;
			}
			else if (v < 0)
			{
				destPtr[0] = 0;
				destPtr[1] = 0;
				destPtr[2] = 0;
			}
			else
			{
				destPtr[0] = (UInt8)Math::Double2Int32(v);
				destPtr[1] = (UInt8)Math::Double2Int32(v);
				destPtr[2] = (UInt8)Math::Double2Int32(v);
			}
			destPtr[3] = 255;
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	dbpl -= w << 3;
	sbpl -= w >> 3;
	while (h-- > 0)
	{
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			cPtr = &pal[((v >> 7) & 1) << 2];
			destPtr[0] = cPtr[0];
			destPtr[1] = cPtr[0];
			destPtr[2] = cPtr[1];
			destPtr[3] = cPtr[1];
			destPtr[4] = cPtr[2];
			destPtr[5] = cPtr[2];
			destPtr[6] = cPtr[3];
			destPtr[7] = cPtr[3];
			cPtr = &pal[((v >> 6) & 1) << 2];
			destPtr[8] = cPtr[0];
			destPtr[9] = cPtr[0];
			destPtr[10] = cPtr[1];
			destPtr[11] = cPtr[1];
			destPtr[12] = cPtr[2];
			destPtr[13] = cPtr[2];
			destPtr[14] = cPtr[3];
			destPtr[15] = cPtr[3];
			cPtr = &pal[((v >> 5) & 1) << 2];
			destPtr[16] = cPtr[0];
			destPtr[17] = cPtr[0];
			destPtr[18] = cPtr[1];
			destPtr[19] = cPtr[1];
			destPtr[20] = cPtr[2];
			destPtr[21] = cPtr[2];
			destPtr[22] = cPtr[3];
			destPtr[23] = cPtr[3];
			cPtr = &pal[((v >> 4) & 1) << 2];
			destPtr[24] = cPtr[0];
			destPtr[25] = cPtr[0];
			destPtr[26] = cPtr[1];
			destPtr[27] = cPtr[1];
			destPtr[28] = cPtr[2];
			destPtr[29] = cPtr[2];
			destPtr[30] = cPtr[3];
			destPtr[31] = cPtr[3];
			cPtr = &pal[((v >> 3) & 1) << 2];
			destPtr[32] = cPtr[0];
			destPtr[33] = cPtr[0];
			destPtr[34] = cPtr[1];
			destPtr[35] = cPtr[1];
			destPtr[36] = cPtr[2];
			destPtr[37] = cPtr[2];
			destPtr[38] = cPtr[3];
			destPtr[39] = cPtr[3];
			cPtr = &pal[((v >> 2) & 1) << 2];
			destPtr[40] = cPtr[0];
			destPtr[41] = cPtr[0];
			destPtr[42] = cPtr[1];
			destPtr[43] = cPtr[1];
			destPtr[44] = cPtr[2];
			destPtr[45] = cPtr[2];
			destPtr[46] = cPtr[3];
			destPtr[47] = cPtr[3];
			cPtr = &pal[((v >> 1) & 1) << 2];
			destPtr[48] = cPtr[0];
			destPtr[49] = cPtr[0];
			destPtr[50] = cPtr[1];
			destPtr[51] = cPtr[1];
			destPtr[52] = cPtr[2];
			destPtr[53] = cPtr[2];
			destPtr[54] = cPtr[3];
			destPtr[55] = cPtr[3];
			cPtr = &pal[(v & 1) << 2];
			destPtr[56] = cPtr[0];
			destPtr[57] = cPtr[0];
			destPtr[58] = cPtr[1];
			destPtr[59] = cPtr[1];
			destPtr[60] = cPtr[2];
			destPtr[61] = cPtr[2];
			destPtr[62] = cPtr[3];
			destPtr[63] = cPtr[3];
			destPtr += 64;
		}

		i = w & 7;
		if (i > 0)
		{
			v = *srcPtr;
			while (i-- > 0)
			{
				cPtr = &pal[(v >> 7) << 2];
				destPtr[0] = cPtr[0];
				destPtr[1] = cPtr[0];
				destPtr[2] = cPtr[1];
				destPtr[3] = cPtr[1];
				destPtr[4] = cPtr[2];
				destPtr[5] = cPtr[2];
				destPtr[6] = cPtr[3];
				destPtr[7] = cPtr[3];
				destPtr += 8;
				v = v << 1;
			}
		}

		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP2_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	dbpl -= w << 3;
	sbpl -= w >> 2;
	while (h-- > 0)
	{
		i = w >> 2;
		while (i-- > 0)
		{
			v = *srcPtr++;
			cPtr = &pal[((v >> 6) & 3) << 2];
			destPtr[0] = cPtr[0];
			destPtr[1] = cPtr[0];
			destPtr[2] = cPtr[1];
			destPtr[3] = cPtr[1];
			destPtr[4] = cPtr[2];
			destPtr[5] = cPtr[2];
			destPtr[6] = cPtr[3];
			destPtr[7] = cPtr[3];
			cPtr = &pal[((v >> 4) & 3) << 2];
			destPtr[8] = cPtr[0];
			destPtr[9] = cPtr[0];
			destPtr[10] = cPtr[1];
			destPtr[11] = cPtr[1];
			destPtr[12] = cPtr[2];
			destPtr[13] = cPtr[2];
			destPtr[14] = cPtr[3];
			destPtr[15] = cPtr[3];
			cPtr = &pal[((v >> 2) & 3) << 2];
			destPtr[16] = cPtr[0];
			destPtr[17] = cPtr[0];
			destPtr[18] = cPtr[1];
			destPtr[19] = cPtr[1];
			destPtr[20] = cPtr[2];
			destPtr[21] = cPtr[2];
			destPtr[22] = cPtr[3];
			destPtr[23] = cPtr[3];
			cPtr = &pal[(v & 3) << 2];
			destPtr[24] = cPtr[0];
			destPtr[25] = cPtr[0];
			destPtr[26] = cPtr[1];
			destPtr[27] = cPtr[1];
			destPtr[28] = cPtr[2];
			destPtr[29] = cPtr[2];
			destPtr[30] = cPtr[3];
			destPtr[31] = cPtr[3];
			destPtr += 32;
		}

		i = w & 3;
		if (i > 0)
		{
			v = *srcPtr;
			while (i-- > 0)
			{
				cPtr = &pal[(v >> 6) << 2];
				destPtr[0] = cPtr[0];
				destPtr[1] = cPtr[0];
				destPtr[2] = cPtr[1];
				destPtr[3] = cPtr[1];
				destPtr[4] = cPtr[2];
				destPtr[5] = cPtr[2];
				destPtr[6] = cPtr[3];
				destPtr[7] = cPtr[3];
				destPtr += 8;
				v = v << 2;
			}
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP4_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	if (w & 1)
	{
		dbpl -= w << 3;
		w = w >> 1;
		sbpl -= w;
		while (h-- > 0)
		{
			i = w;
			while (i-- > 0)
			{
				cPtr = &pal[(*srcPtr >> 4) << 2];
				destPtr[0] = cPtr[0];
				destPtr[1] = cPtr[0];
				destPtr[2] = cPtr[1];
				destPtr[3] = cPtr[1];
				destPtr[4] = cPtr[2];
				destPtr[5] = cPtr[2];
				destPtr[6] = cPtr[3];
				destPtr[7] = cPtr[3];
				cPtr = &pal[(*srcPtr & 15) << 2];
				destPtr[8] = cPtr[0];
				destPtr[9] = cPtr[0];
				destPtr[10] = cPtr[1];
				destPtr[11] = cPtr[1];
				destPtr[12] = cPtr[2];
				destPtr[13] = cPtr[2];
				destPtr[14] = cPtr[3];
				destPtr[15] = cPtr[3];
				srcPtr += 1;
				destPtr += 16;
			}

			cPtr = &pal[(*srcPtr >> 4) << 2];
			destPtr[0] = cPtr[0];
			destPtr[1] = cPtr[0];
			destPtr[2] = cPtr[1];
			destPtr[3] = cPtr[1];
			destPtr[4] = cPtr[2];
			destPtr[5] = cPtr[2];
			destPtr[6] = cPtr[3];
			destPtr[7] = cPtr[3];
			destPtr += 8;

			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
	else
	{
		dbpl -= w << 3;
		w = w >> 1;
		sbpl -= w;
		while (h-- > 0)
		{
			i = w;
			while (i-- > 0)
			{
				cPtr = &pal[(*srcPtr >> 4) << 2];
				destPtr[0] = cPtr[0];
				destPtr[1] = cPtr[0];
				destPtr[2] = cPtr[1];
				destPtr[3] = cPtr[1];
				destPtr[4] = cPtr[2];
				destPtr[5] = cPtr[2];
				destPtr[6] = cPtr[3];
				destPtr[7] = cPtr[3];
				cPtr = &pal[(*srcPtr & 15) << 2];
				destPtr[8] = cPtr[0];
				destPtr[9] = cPtr[0];
				destPtr[10] = cPtr[1];
				destPtr[11] = cPtr[1];
				destPtr[12] = cPtr[2];
				destPtr[13] = cPtr[2];
				destPtr[14] = cPtr[3];
				destPtr[15] = cPtr[3];
				srcPtr += 1;
				destPtr += 16;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
}

extern "C" void ImageUtil_ConvP8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	sbpl -= w;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			cPtr = &pal[*srcPtr << 2];
			destPtr[0] = cPtr[0];
			destPtr[1] = cPtr[0];
			destPtr[2] = cPtr[1];
			destPtr[3] = cPtr[1];
			destPtr[4] = cPtr[2];
			destPtr[5] = cPtr[2];
			destPtr[6] = cPtr[3];
			destPtr[7] = cPtr[3];
			srcPtr += 1;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP1_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	UInt8 *dptr;
	UInt8 a;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			cPtr = &pal[((v >> 7) & 1) << 2];
			dptr[0] = cPtr[0];
			dptr[1] = cPtr[0];
			dptr[2] = cPtr[1];
			dptr[3] = cPtr[1];
			dptr[4] = cPtr[2];
			dptr[5] = cPtr[2];
			dptr[6] = cPtr[3];
			dptr[7] = cPtr[3];
			cPtr = &pal[((v >> 6) & 1) << 2];
			dptr[8] = cPtr[0];
			dptr[9] = cPtr[0];
			dptr[10] = cPtr[1];
			dptr[11] = cPtr[1];
			dptr[12] = cPtr[2];
			dptr[13] = cPtr[2];
			dptr[14] = cPtr[3];
			dptr[15] = cPtr[3];
			cPtr = &pal[((v >> 5) & 1) << 2];
			dptr[16] = cPtr[0];
			dptr[17] = cPtr[0];
			dptr[18] = cPtr[1];
			dptr[19] = cPtr[1];
			dptr[20] = cPtr[2];
			dptr[21] = cPtr[2];
			dptr[22] = cPtr[3];
			dptr[23] = cPtr[3];
			cPtr = &pal[((v >> 4) & 1) << 2];
			dptr[24] = cPtr[0];
			dptr[25] = cPtr[0];
			dptr[26] = cPtr[1];
			dptr[27] = cPtr[1];
			dptr[28] = cPtr[2];
			dptr[29] = cPtr[2];
			dptr[30] = cPtr[3];
			dptr[31] = cPtr[3];
			cPtr = &pal[((v >> 3) & 1) << 2];
			dptr[32] = cPtr[0];
			dptr[33] = cPtr[0];
			dptr[34] = cPtr[1];
			dptr[35] = cPtr[1];
			dptr[36] = cPtr[2];
			dptr[37] = cPtr[2];
			dptr[38] = cPtr[3];
			dptr[39] = cPtr[3];
			cPtr = &pal[((v >> 2) & 1) << 2];
			dptr[40] = cPtr[0];
			dptr[41] = cPtr[0];
			dptr[42] = cPtr[1];
			dptr[43] = cPtr[1];
			dptr[44] = cPtr[2];
			dptr[45] = cPtr[2];
			dptr[46] = cPtr[3];
			dptr[47] = cPtr[3];
			cPtr = &pal[((v >> 1) & 1) << 2];
			dptr[48] = cPtr[0];
			dptr[49] = cPtr[0];
			dptr[50] = cPtr[1];
			dptr[51] = cPtr[1];
			dptr[52] = cPtr[2];
			dptr[53] = cPtr[2];
			dptr[54] = cPtr[3];
			dptr[55] = cPtr[3];
			cPtr = &pal[(v & 1) << 2];
			dptr[56] = cPtr[0];
			dptr[57] = cPtr[0];
			dptr[58] = cPtr[1];
			dptr[59] = cPtr[1];
			dptr[60] = cPtr[2];
			dptr[61] = cPtr[2];
			dptr[62] = cPtr[3];
			dptr[63] = cPtr[3];
			dptr += 64;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				cPtr = &pal[((v >> 7) & 1) << 2];
				dptr[0] = cPtr[0];
				dptr[1] = cPtr[0];
				dptr[2] = cPtr[1];
				dptr[3] = cPtr[1];
				dptr[4] = cPtr[2];
				dptr[5] = cPtr[2];
				dptr[6] = cPtr[3];
				dptr[7] = cPtr[3];
				dptr += 8;
				v = v << 1;
			}
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[6] = a = -((v >> 7) & 1);
			dptr[7] = a;
			dptr[14] = a = -((v >> 6) & 1);
			dptr[15] = a;
			dptr[22] = a = -((v >> 5) & 1);
			dptr[23] = a;
			dptr[30] = a = -((v >> 4) & 1);
			dptr[31] = a;
			dptr[38] = a = -((v >> 3) & 1);
			dptr[39] = a;
			dptr[46] = a = -((v >> 2) & 1);
			dptr[47] = a;
			dptr[54] = a = -((v >> 1) & 1);
			dptr[55] = a;
			dptr[62] = a = -(v & 1);
			dptr[63] = a;
			dptr += 64;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[6] = a = -((v >> 7) & 1);
				dptr[7] = a;
				dptr += 8;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP2_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	UInt8 *dptr;
	UInt8 a;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 2;
		while (i-- > 0)
		{
			v = *srcPtr++;
			cPtr = &pal[((v >> 6) & 3) << 2];
			dptr[0] = cPtr[0];
			dptr[1] = cPtr[0];
			dptr[2] = cPtr[1];
			dptr[3] = cPtr[1];
			dptr[4] = cPtr[2];
			dptr[5] = cPtr[2];
			dptr[6] = cPtr[3];
			dptr[7] = cPtr[3];
			cPtr = &pal[((v >> 4) & 3) << 2];
			dptr[8] = cPtr[0];
			dptr[9] = cPtr[0];
			dptr[10] = cPtr[1];
			dptr[11] = cPtr[1];
			dptr[12] = cPtr[2];
			dptr[13] = cPtr[2];
			dptr[14] = cPtr[3];
			dptr[15] = cPtr[3];
			cPtr = &pal[((v >> 2) & 3) << 2];
			dptr[16] = cPtr[0];
			dptr[17] = cPtr[0];
			dptr[18] = cPtr[1];
			dptr[19] = cPtr[1];
			dptr[20] = cPtr[2];
			dptr[21] = cPtr[2];
			dptr[22] = cPtr[3];
			dptr[23] = cPtr[3];
			cPtr = &pal[(v & 3) << 2];
			dptr[24] = cPtr[0];
			dptr[25] = cPtr[0];
			dptr[26] = cPtr[1];
			dptr[27] = cPtr[1];
			dptr[28] = cPtr[2];
			dptr[29] = cPtr[2];
			dptr[30] = cPtr[3];
			dptr[31] = cPtr[3];
			dptr += 32;
		}
		if (w & 3)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				cPtr = &pal[((v >> 6) & 3) << 2];
				dptr[0] = cPtr[0];
				dptr[1] = cPtr[0];
				dptr[2] = cPtr[1];
				dptr[3] = cPtr[1];
				dptr[4] = cPtr[2];
				dptr[5] = cPtr[2];
				dptr[6] = cPtr[3];
				dptr[7] = cPtr[3];
				dptr += 8;
				v = v << 1;
			}
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[6] = a = -((v >> 7) & 1);
			dptr[7] = a;
			dptr[14] = a = -((v >> 6) & 1);
			dptr[15] = a;
			dptr[22] = a = -((v >> 5) & 1);
			dptr[23] = a;
			dptr[30] = a = -((v >> 4) & 1);
			dptr[31] = a;
			dptr[38] = a = -((v >> 3) & 1);
			dptr[39] = a;
			dptr[46] = a = -((v >> 2) & 1);
			dptr[47] = a;
			dptr[54] = a = -((v >> 1) & 1);
			dptr[55] = a;
			dptr[62] = a = -(v & 1);
			dptr[63] = a;
			dptr += 64;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[6] = a = -((v >> 7) & 1);
				dptr[7] = a;
				dptr += 8;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP4_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	UInt8 *dptr;
	UInt8 a;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w >> 1;
		while (i-- > 0)
		{
			cPtr = &pal[(*srcPtr >> 4) << 2];
			dptr[0] = cPtr[0];
			dptr[1] = cPtr[0];
			dptr[2] = cPtr[1];
			dptr[3] = cPtr[1];
			dptr[4] = cPtr[2];
			dptr[5] = cPtr[2];
			dptr[6] = cPtr[3];
			dptr[7] = cPtr[3];
			cPtr = &pal[(*srcPtr & 15) << 2];
			dptr[8] = cPtr[0];
			dptr[9] = cPtr[0];
			dptr[10] = cPtr[1];
			dptr[11] = cPtr[1];
			dptr[12] = cPtr[2];
			dptr[13] = cPtr[2];
			dptr[14] = cPtr[3];
			dptr[15] = cPtr[3];
			srcPtr += 1;
			dptr += 16;
		}
		if (w & 1)
		{
			cPtr = &pal[(srcPtr[0] >> 4) << 2];
			srcPtr++;
			dptr[0] = cPtr[0];
			dptr[1] = cPtr[0];
			dptr[2] = cPtr[1];
			dptr[3] = cPtr[1];
			dptr[4] = cPtr[2];
			dptr[5] = cPtr[2];
			dptr[6] = cPtr[3];
			dptr[7] = cPtr[3];
			dptr += 8;
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[6] = a = -((v >> 7) & 1);
			dptr[7] = a;
			dptr[14] = a = -((v >> 6) & 1);
			dptr[15] = a;
			dptr[22] = a = -((v >> 5) & 1);
			dptr[23] = a;
			dptr[30] = a = -((v >> 4) & 1);
			dptr[31] = a;
			dptr[38] = a = -((v >> 3) & 1);
			dptr[39] = a;
			dptr[46] = a = -((v >> 2) & 1);
			dptr[47] = a;
			dptr[54] = a = -((v >> 1) & 1);
			dptr[55] = a;
			dptr[62] = a = -(v & 1);
			dptr[63] = a;
			dptr += 64;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[6] = a = -((v >> 7) & 1);
				dptr[7] = a;
				dptr += 8;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvP8_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal)
{
	OSInt i;
	const UInt8 *cPtr;
	UInt8 v;
	UInt8 *dptr;
	UInt8 a;
	while (h-- > 0)
	{
		dptr = destPtr;
		i = w;
		while (i-- > 0)
		{
			cPtr = &pal[*srcPtr << 2];
			dptr[0] = cPtr[0];
			dptr[1] = cPtr[0];
			dptr[2] = cPtr[1];
			dptr[3] = cPtr[1];
			dptr[4] = cPtr[2];
			dptr[5] = cPtr[2];
			dptr[6] = cPtr[3];
			dptr[7] = cPtr[3];
			srcPtr += 1;
			dptr += 8;
		}

		dptr = destPtr;
		i = w >> 3;
		while (i-- > 0)
		{
			v = *srcPtr++;
			dptr[6] = a = -((v >> 7) & 1);
			dptr[7] = a;
			dptr[14] = a = -((v >> 6) & 1);
			dptr[15] = a;
			dptr[22] = a = -((v >> 5) & 1);
			dptr[23] = a;
			dptr[30] = a = -((v >> 4) & 1);
			dptr[31] = a;
			dptr[38] = a = -((v >> 3) & 1);
			dptr[39] = a;
			dptr[46] = a = -((v >> 2) & 1);
			dptr[47] = a;
			dptr[54] = a = -((v >> 1) & 1);
			dptr[55] = a;
			dptr[62] = a = -(v & 1);
			dptr[63] = a;
			dptr += 64;
		}
		if (w & 7)
		{
			v = srcPtr[0];
			srcPtr++;
			i = w & 7;
			while (i-- > 0)
			{
				dptr[6] = a = -((v >> 7) & 1);
				dptr[7] = a;
				dptr += 8;
				v = v << 1;
			}
		}

		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB5G5R5_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	UInt8 cv;
	UInt16 v;
	sbpl -= w << 1;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(srcPtr);
			cv = v & 0x1f;
			destPtr[0] = (cv << 6) | (cv << 1) | (cv >> 4);
			destPtr[1] = (cv << 3) | (cv >> 2);
			cv = (v >> 5) & 0x1f;
			destPtr[2] = (cv << 6) | (cv << 1) | (cv >> 4);
			destPtr[3] = (cv << 3) | (cv >> 2);
			cv = v >> 10;
			destPtr[4] = (cv << 6) | (cv << 1) | (cv >> 4);
			destPtr[5] = (cv << 3) | (cv >> 2);
			destPtr[6] = 0xff;
			destPtr[7] = 0xff;
			srcPtr += 2;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB5G6R5_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	UInt8 cv;
	UInt16 v;
	sbpl -= w << 1;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(srcPtr);
			cv = v & 0x1f;
			destPtr[0] = (cv << 6) | (cv << 1) | (cv >> 4);
			destPtr[1] = (cv << 3) | (cv >> 2);
			cv = (v >> 5) & 0x3f;
			destPtr[2] = (cv << 4) | (cv >> 2);
			destPtr[3] = (cv << 2) | (cv >> 4);
			cv = v >> 11;
			destPtr[4] = (cv << 6) | (cv << 1) | (cv >> 4);
			destPtr[5] = (cv << 3) | (cv >> 2);
			destPtr[6] = 0xff;
			destPtr[7] = 0xff;
			srcPtr += 2;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvB8G8R8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 3;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[0];
			destPtr[1] = srcPtr[0];
			destPtr[2] = srcPtr[1];
			destPtr[3] = srcPtr[1];
			destPtr[4] = srcPtr[2];
			destPtr[5] = srcPtr[2];
			destPtr[6] = 0xff;
			destPtr[7] = 0xff;
			srcPtr += 3;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvR8G8B8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 3;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[2];
			destPtr[1] = srcPtr[2];
			destPtr[2] = srcPtr[1];
			destPtr[3] = srcPtr[1];
			destPtr[4] = srcPtr[0];
			destPtr[5] = srcPtr[0];
			destPtr[6] = 0xff;
			destPtr[7] = 0xff;
			srcPtr += 3;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvARGB32_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w << 2;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[0];
			destPtr[1] = srcPtr[0];
			destPtr[2] = srcPtr[1];
			destPtr[3] = srcPtr[1];
			destPtr[4] = srcPtr[2];
			destPtr[5] = srcPtr[2];
			destPtr[6] = srcPtr[3];
			destPtr[7] = srcPtr[3];
			srcPtr += 4;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvARGB48_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	sbpl -= w * 6;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			destPtr[0] = srcPtr[0];
			destPtr[1] = srcPtr[1];
			destPtr[2] = srcPtr[2];
			destPtr[3] = srcPtr[3];
			destPtr[4] = srcPtr[4];
			destPtr[5] = srcPtr[5];
			destPtr[6] = 0xff;
			destPtr[7] = 0xff;
			srcPtr += 6;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvA2B10G10R10_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt16 r;
	UInt16 g;
	UInt16 b;
	UInt16 a;
	UInt32 v;
	OSInt i;
	sbpl -= w << 2;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt32(srcPtr);
			r = v & 0x3ff;
			g = (v >> 10) & 0x3ff;
			b = (v >> 20) & 0x3ff;
			a = (v >> 30) & 3;
			r = (r << 6) | (r >> 4);
			g = (g << 6) | (g >> 4);
			b = (b << 6) | (b >> 4);
			a = (a << 2) | a;
			a = (a << 4) | a;
			a = (a << 8) | a;
			WriteInt16(&destPtr[0], b);
			WriteInt16(&destPtr[2], g);
			WriteInt16(&destPtr[4], r);
			WriteInt16(&destPtr[6], a);
			srcPtr += 4;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFB32G32R32A32_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w << 4;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[0], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[0], 0);
			}
			else
			{
				WriteInt16(&destPtr[0], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[4]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[2], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[2], 0);
			}
			else
			{
				WriteInt16(&destPtr[2], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[8]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[4], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[4], 0);
			}
			else
			{
				WriteInt16(&destPtr[4], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[12]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[6], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[6], 0);
			}
			else
			{
				WriteInt16(&destPtr[6], Math::Double2Int32(v));
			}
			srcPtr += 16;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFB32G32R32_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w * 12;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[0], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[0], 0);
			}
			else
			{
				WriteInt16(&destPtr[0], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[4]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[2], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[2], 0);
			}
			else
			{
				WriteInt16(&destPtr[2], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[8]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[4], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[4], 0);
			}
			else
			{
				WriteInt16(&destPtr[4], Math::Double2Int32(v));
			}
			WriteInt16(&destPtr[6], 65535);
			srcPtr += 12;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFW32A32_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w * 8;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[0], 65535);
				WriteInt16(&destPtr[2], 65535);
				WriteInt16(&destPtr[4], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[0], 0);
				WriteInt16(&destPtr[2], 0);
				WriteInt16(&destPtr[4], 0);
			}
			else
			{
				WriteInt16(&destPtr[0], Math::Double2Int32(v));
				WriteInt16(&destPtr[2], Math::Double2Int32(v));
				WriteInt16(&destPtr[4], Math::Double2Int32(v));
			}
			v = ReadFloat(&srcPtr[4]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[6], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[6], 0);
			}
			else
			{
				WriteInt16(&destPtr[6], Math::Double2Int32(v));
			}
			srcPtr += 8;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvFW32_64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	Single v;
	OSInt i;
	sbpl -= w * 4;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadFloat(&srcPtr[0]) * 65535;
			if (v > 65535)
			{
				WriteInt16(&destPtr[0], 65535);
				WriteInt16(&destPtr[2], 65535);
				WriteInt16(&destPtr[4], 65535);
			}
			else if (v < 0)
			{
				WriteInt16(&destPtr[0], 0);
				WriteInt16(&destPtr[2], 0);
				WriteInt16(&destPtr[4], 0);
			}
			else
			{
				WriteInt16(&destPtr[0], Math::Double2Int32(v));
				WriteInt16(&destPtr[2], Math::Double2Int32(v));
				WriteInt16(&destPtr[4], Math::Double2Int32(v));
			}
			WriteInt16(&destPtr[6], 65535);
			srcPtr += 4;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW16_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt8 v;
	OSInt i;
	sbpl -= w * 2;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = srcPtr[1];
			destPtr[0] = v;
			destPtr[1] = v;
			destPtr[2] = v;
			destPtr[3] = 0xff;
			srcPtr += 2;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW16A16_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt8 v;
	OSInt i;
	sbpl -= w * 4;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = srcPtr[1];
			destPtr[0] = v;
			destPtr[1] = v;
			destPtr[2] = v;
			destPtr[3] = srcPtr[3];
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW8A8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt8 v;
	OSInt i;
	sbpl -= w * 2;
	dbpl -= w << 2;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = srcPtr[0];
			destPtr[0] = v;
			destPtr[1] = v;
			destPtr[2] = v;
			destPtr[3] = srcPtr[1];
			srcPtr += 2;
			destPtr += 4;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW16_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt16 v;
	OSInt i;
	sbpl -= w * 2;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(&srcPtr[0]);
			WriteInt16(&destPtr[0], v);
			WriteInt16(&destPtr[2], v);
			WriteInt16(&destPtr[4], v);
			WriteInt16(&destPtr[6], 0xffff);
			srcPtr += 2;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW16A16_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt16 v;
	OSInt i;
	sbpl -= w * 4;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = ReadUInt16(&srcPtr[0]);
			WriteInt16(&destPtr[0], v);
			WriteInt16(&destPtr[2], v);
			WriteInt16(&destPtr[4], v);
			v = ReadUInt16(&srcPtr[2]);
			WriteInt16(&destPtr[6], v);
			srcPtr += 4;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_ConvW8A8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, OSInt w, OSInt h, OSInt sbpl, OSInt dbpl)
{
	UInt16 v;
	OSInt i;
	sbpl -= w * 2;
	dbpl -= w << 3;
	while (h-- > 0)
	{
		i = w;
		while (i-- > 0)
		{
			v = (((UInt16)srcPtr[0]) << 8) | srcPtr[0];
			WriteInt16(&destPtr[0], v);
			WriteInt16(&destPtr[2], v);
			WriteInt16(&destPtr[4], v);
			v = (((UInt16)srcPtr[1]) << 8) | srcPtr[1];
			WriteInt16(&destPtr[6], v);
			srcPtr += 2;
			destPtr += 8;
		}
		srcPtr += sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate32_CW90(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	const UInt8 *sptr;
	srcPtr += sbpl * srcHeight;
	dbpl -= srcHeight << 2;
	while (srcWidth-- > 0)
	{
		OSInt wLeft = srcHeight;
		sptr = srcPtr;
		while (wLeft-- > 0)
		{
			sptr -= sbpl;
			*(Int32*)destPtr = *(Int32*)sptr;
			destPtr += 4;
		}
		srcPtr += 4;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate32_CW180(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	srcPtr += sbpl * srcHeight;
	sbpl -= srcWidth << 2;
	dbpl -= srcWidth << 2;
	while (srcHeight-- > 0)
	{
		OSInt wLeft = srcWidth;
		while (wLeft-- > 0)
		{
			srcPtr -= 4;
			*(Int32*)destPtr = *(Int32*)srcPtr;
			destPtr += 4;
		}
		srcPtr -= sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate32_CW270(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	srcPtr += sbpl;
	dbpl -= srcHeight << 2;
	const UInt8 *sptr;
	while (srcWidth-- > 0)
	{
		srcPtr -= 4;
		sptr = srcPtr;
		OSInt wLeft = srcHeight;
		while (wLeft-- > 0)
		{
			*(Int32*)destPtr = *(Int32*)sptr;
			sptr += sbpl;
			destPtr += 4;
		}
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate64_CW90(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	const UInt8 *sptr;
	srcPtr += sbpl * srcHeight;
	dbpl -= srcHeight << 3;
	while (srcWidth-- > 0)
	{
		OSInt wLeft = srcHeight;
		sptr = srcPtr;
		while (wLeft-- > 0)
		{
			sptr -= sbpl;
			*(Int64*)destPtr = *(Int64*)sptr;
			destPtr += 8;
		}
		srcPtr += 8;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate64_CW180(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	srcPtr += sbpl * srcHeight;
	sbpl -= srcWidth << 3;
	dbpl -= srcWidth << 3;
	while (srcHeight-- > 0)
	{
		OSInt wLeft = srcWidth;
		while (wLeft-- > 0)
		{
			srcPtr -= 8;
			*(Int64*)destPtr = *(Int64*)srcPtr;
			destPtr += 8;
		}
		srcPtr -= sbpl;
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_Rotate64_CW270(const UInt8 *srcPtr, UInt8 *destPtr, OSInt srcWidth, OSInt srcHeight, OSInt sbpl, OSInt dbpl)
{
	srcPtr += sbpl;
	dbpl -= srcHeight << 3;
	const UInt8 *sptr;
	while (srcWidth-- > 0)
	{
		srcPtr -= 8;
		sptr = srcPtr;
		OSInt wLeft = srcHeight;
		while (wLeft-- > 0)
		{
			*(Int64*)destPtr = *(Int64*)sptr;
			sptr += sbpl;
			destPtr += 8;
		}
		destPtr += dbpl;
	}
}

extern "C" void ImageUtil_CopyShiftW(const UInt8 *srcPtr, UInt8 *destPtr, OSInt byteSize, OSInt shiftCnt)
{
	byteSize = byteSize >> 1;
	while (byteSize-- > 0)
	{
		WriteInt16(destPtr, ReadUInt16(srcPtr) << shiftCnt);
		destPtr += 2;
		srcPtr += 2;
	}
}

extern "C" void ImageUtil_UVInterleaveShiftW(UInt8 *destPtr, const UInt8 *uptr, const UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt)
{
	while (pixelCnt-- > 0)
	{
		WriteInt16(&destPtr[0], ReadUInt16(uptr) << shiftCnt);
		WriteInt16(&destPtr[2], ReadUInt16(vptr) << shiftCnt);
		destPtr += 4;
		uptr += 2;
		vptr += 2;
	}
}

extern "C" void ImageUtil_YUV_Y416ShiftW(UInt8 *destPtr, const UInt8 *yptr, const UInt8 *uptr, const UInt8 *vptr, OSInt pixelCnt, OSInt shiftCnt)
{
	while (pixelCnt-- > 0)
	{
		WriteInt16(&destPtr[0], ReadUInt16(uptr) << shiftCnt);
		WriteInt16(&destPtr[2], ReadUInt16(yptr) << shiftCnt);
		WriteInt16(&destPtr[4], ReadUInt16(vptr) << shiftCnt);
		WriteInt16(&destPtr[6], 0xffff);
		destPtr += 8;
		yptr += 2;
		uptr += 2;
		vptr += 2;
	}
}
