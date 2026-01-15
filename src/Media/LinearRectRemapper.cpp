#include "Stdafx.h"
#include "Media/LinearRectRemapper.h"

NN<Media::StaticImage> Media::LinearRectRemapper::RemapW8(UnsafeArray<const UInt8> imgPtr, Math::Size2D<UIntOS> imgSize, IntOS imgBpl, Math::Size2D<UIntOS> outputSize, Math::Quadrilateral quad, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::YCOffset ycOfst)
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
	IntOS ix;
	IntOS iy;
	IntOS ofst;
	IntOS imgXEnd = (IntOS)imgSize.x - 1;
	IntOS imgYEnd = (IntOS)imgSize.y - 1;
	UIntOS i;
	UIntOS j;

	NN<Media::StaticImage> ret;
	NEW_CLASSNN(ret, Media::StaticImage(outputSize, 0, 8, Media::PF_PAL_W8, outputSize.CalcArea(), color, yuvType, Media::AT_ALPHA_ALL_FF, ycOfst));
	ret->InitGrayPal();
	xPos = UIntOS2Double(outputSize.x - 1);
	yPos = UIntOS2Double(outputSize.y - 1);
	UnsafeArray<UInt8> dptr = ret->data;
	i = 0;
	while (i < outputSize.y)
	{
		yRate = UIntOS2Double(i) / yPos;

		j = 0;
		while (j < outputSize.x)
		{
			xRate = UIntOS2Double(j) / xPos;
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
			ix = (IntOS)pt.x;
			iy = (IntOS)pt.y;
			ofst = iy * imgBpl + ix;
			rRate = pt.x - IntOS2Double(ix);
			lRate = 1 - rRate;
			bRate = pt.y - IntOS2Double(iy);
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
