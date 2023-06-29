#include "Stdafx.h"
#include "Media/LinearRectRemapper.h"

Media::StaticImage *Media::LinearRectRemapper::RemapW8(const UInt8 *imgPtr, Math::Size2D<UOSInt> imgSize, OSInt imgBpl, Math::Size2D<UOSInt> outputSize, Math::Quadrilateral quad, Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::YCOffset ycOfst)
{
	Math::Coord2DDbl pt;
	Double xPos;
	Double yPos;
	Double xRate;
	Double yRate;
	Double lRate;
	Double rRate;
	Double tRate;
	Double bRate;
	Double c;
	OSInt ix;
	OSInt iy;
	OSInt ofst;
	OSInt imgXEnd = (OSInt)imgSize.x - 1;
	OSInt imgYEnd = (OSInt)imgSize.y - 1;
	UOSInt i;
	UOSInt j;

	Media::StaticImage *ret;
	NEW_CLASS(ret, Media::StaticImage(outputSize, 0, 8, Media::PF_PAL_W8, outputSize.CalcArea(), color, yuvType, Media::AT_NO_ALPHA, ycOfst));
	ret->InitGrayPal();
	xPos = UOSInt2Double(outputSize.x - 1);
	yPos = UOSInt2Double(outputSize.y - 1);
	UInt8 *dptr = ret->data;
	i = 0;
	while (i < outputSize.y)
	{
		yRate = UOSInt2Double(i) / yPos;

		j = 0;
		while (j < outputSize.x)
		{
			xRate = UOSInt2Double(j) / xPos;
			if (xRate + yRate <= 1)
			{
				pt = quad.tl + (quad.tr - quad.tl) * xRate + (quad.bl - quad.tl) * yRate;
			}
			else
			{
				pt = quad.br + (quad.tr - quad.br) * (1 - yRate) + (quad.bl - quad.br) * (1 - xRate);
			}
			if (pt.x < 0)
			{
				pt.x = 0;
			}
			if (pt.y < 0)
			{
				pt.y = 0;
			}
			ix = (OSInt)pt.x;
			iy = (OSInt)pt.y;
			ofst = iy * imgBpl + ix;
			rRate = pt.x - OSInt2Double(ix);
			lRate = 1 - rRate;
			bRate = pt.y - OSInt2Double(iy);
			tRate = 1 - bRate;
			if (ix < imgXEnd)
			{
				if (iy < imgYEnd)
				{
					c = (imgPtr[ofst] * lRate + imgPtr[ofst + 1] * rRate) * tRate;
					c += (imgPtr[ofst + imgBpl] * lRate + imgPtr[ofst + imgBpl + 1] * rRate) * bRate;
				}
				else
				{
					c = (imgPtr[ofst] * lRate + imgPtr[ofst + 1] * rRate);
				}
			}
			else
			{
				if (iy < imgYEnd)
				{
					c = (imgPtr[ofst] * tRate + imgPtr[ofst + imgBpl] * bRate);
				}
				else
				{
					c = imgPtr[ofst];
				}
			}
			*dptr = (UInt8)(UInt32)(c + 0.5);
			dptr++;
			j++;
		}
		i++;
	}
	return ret;
}
