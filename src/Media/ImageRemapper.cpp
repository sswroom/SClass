#include "Stdafx.h"
#include "Math/Triangle.h"
#include "Media/ImageRemapper.h"

Media::ImageRemapper::ImageRemapper()
{
	this->srcImgPtr = 0;
	this->srcBpl = 0;
	this->srcWidth = 0;
	this->srcHeight = 0;
	this->srcPF = Media::PF_UNKNOWN;
	this->srcPal = 0;
}

void Media::ImageRemapper::SetSourceImage32(UnsafeArray<const UInt8> srcImgPtr, UOSInt srcBpl, UOSInt srcWidth, UOSInt srcHeight)
{
	this->srcImgPtr = srcImgPtr;
	this->srcBpl = srcBpl;
	this->srcWidth = srcWidth;
	this->srcHeight = srcHeight;
	this->srcPF = Media::PF_B8G8R8A8;
	this->srcPal = 0;
}

void Media::ImageRemapper::SetSourceImage(NN<Media::StaticImage> srcImg)
{
	this->srcImgPtr = UnsafeArray<const UInt8>(srcImg->data);
	this->srcBpl = srcImg->GetDataBpl();
	this->srcWidth = srcImg->info.dispSize.x;
	this->srcHeight = srcImg->info.dispSize.y;
	this->srcPF = srcImg->info.pf;
	this->srcPal = srcImg->pal;
}

Bool Media::ImageRemapper::Remap(UnsafeArray<UInt8> destImgPtr, UOSInt destBpl, UOSInt destWidth, UOSInt destHeight, Math::Quadrilateral destQuad)
{
	UnsafeArray<const UInt8> srcImgPtr;
	if (!this->srcImgPtr.SetTo(srcImgPtr) || this->srcWidth == 0 || this->srcHeight == 0 || destWidth == 0 || destHeight == 0)
	{
		return false;
	}
	GetPixel32Func getPixel32 = this->GetPixel32();
	if (getPixel32 == nullptr)
	{
		return false;
	}
	Math::RectAreaDbl rect = destQuad.GetExterior();
	OSInt left = (OSInt)rect.min.x;
	OSInt top = (OSInt)rect.min.y;
	OSInt right = (OSInt)rect.max.x;
	OSInt bottom = (OSInt)rect.max.y;
	OSInt x;
	Double y;
	if (left < 0) left = 0;
	if (top < 0) top = 0;
	if (right >= (OSInt)destWidth) right = (OSInt)destWidth - 1;
	if (bottom >= (OSInt)destHeight) bottom = (OSInt)destHeight - 1;
	UOSInt destAdd = destBpl - (UOSInt)(right - left + 1) * 4;
	Math::Quadrilateral quad = destQuad;
	Math::Triangle tlTri = Math::Triangle(quad.tl, quad.tr, quad.bl);
	Math::Triangle brTri = Math::Triangle(quad.br, quad.bl, quad.tr);
	Math::Triangle tlSrc = Math::Triangle(Math::Coord2DDbl(0, 0), Math::Coord2DDbl((Double)this->srcWidth - 1, 0), Math::Coord2DDbl(0, (Double)this->srcHeight - 1));
	Math::Triangle brSrc = Math::Triangle(Math::Coord2DDbl((Double)this->srcWidth - 1, (Double)this->srcHeight - 1), Math::Coord2DDbl(0, (Double)this->srcHeight - 1), Math::Coord2DDbl((Double)this->srcWidth - 1, 0));
	Math::Coord2DDbl coord;
	Double xArr[4];
	Double v;
	Double dright = (Double)right;
	UOSInt ptCnt;
	UOSInt i;
	UOSInt j;
	destImgPtr += (UOSInt)top * destBpl + (UOSInt)left * 4;
	while (top <= bottom)
	{
		y = (Double)top;
		ptCnt = quad.CalcIntersactsAtY(xArr, y);
		j = ptCnt;
		while (j > 1)
		{
			i = 1;
			while (i < j)
			{
				if (xArr[i - 1] > xArr[i])
				{
					v = xArr[i];
					xArr[i] = xArr[i - 1];
					xArr[i - 1] = v;
				}
				i++;
			}
			j--;
		}
		if (ptCnt == 4 || ptCnt == 2)
		{
			if (xArr[1] >= dright)
			{
				xArr[1] = dright;
			}
			x = left;
			while ((Double)x < xArr[0])
			{
				destImgPtr += 4;
				x++;
			}
			while ((Double)x <= xArr[1])
			{
				coord = Math::Coord2DDbl((Double)x, y);
				if (tlTri.InsideOrTouch(coord))
				{
					coord = tlTri.Remap(coord, tlSrc);
				}
				else
				{
					coord = brTri.Remap(coord, brSrc);
				}
				UInt32 px = getPixel32(srcImgPtr, coord, *this);
				WriteNUInt32(&destImgPtr[0], px);
				destImgPtr += 4;
				x++;
			}
			if (ptCnt == 4 && xArr[2] < dright)
			{
				if (xArr[3] >= dright)
				{
					xArr[3] = dright;
				}
				while ((Double)x < xArr[2])
				{
					destImgPtr += 4;
					x++;
				}
				while ((Double)x <= xArr[3])
				{
					coord = Math::Coord2DDbl((Double)x, y);
					if (tlTri.InsideOrTouch(coord))
					{
						coord = tlTri.Remap(coord, tlSrc);
					}
					else
					{
						coord = brTri.Remap(coord, brSrc);
					}
					UInt32 px = getPixel32(srcImgPtr, coord, *this);
					WriteNUInt32(&destImgPtr[0], px);
					destImgPtr += 4;
					x++;
				}
				destImgPtr += 4 * (right - x + 1) ;
			}
			else
			{
				destImgPtr += 4 * (right - x + 1) ;
			}
		}
		else
		{
			destImgPtr += (right - left + 1) * 4;
		}
		destImgPtr += destAdd;
		top++;
	}
	return true;
}
