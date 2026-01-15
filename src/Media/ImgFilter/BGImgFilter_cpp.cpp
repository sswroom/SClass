#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImgFilter/BGImgFilter.h"

extern "C"
{

void BGImgFilter_DiffA8_YUY2(UInt8 *destImg, UInt8 *bgImg, UIntOS pxCnt)
{
	Int32 v;
	while (pxCnt-- > 0)
	{
		v = (destImg[0] - bgImg[0]);
		if (v < 0)
			v = -v;
		destImg[0] = (UInt8)v;

		v = (destImg[1] - bgImg[1]) + 0x80;
		if (v < 0)
			destImg[1] = (UInt8)v;
		else if (v > 255)
			destImg[1] = (UInt8)v;
		else
			destImg[1] = (UInt8)v;

		destImg += 2;
		bgImg += 2;
	}
}

void BGImgFilter_DiffYUV8(UInt8 *destImg, UInt8 *bgImg, UIntOS byteCnt)
{
	Int32 v;
	while (byteCnt-- > 0)
	{
		v = (*destImg - *bgImg) + 0x80;
		if (v < 0)
			*destImg = 0;
		else if (v > 255)
			*destImg = 255;
		else
			*destImg = (UInt8)v;
		destImg++;
		bgImg++;
	}
}
}
