#include "Stdafx.h"
#include "Math/Triangle.h"
#include "Media/NearestNeighbourRemapper.h"

UInt32 Media::NearestNeighbourRemapper::GetPixel32(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord)
{
	UnsafeArray<const UInt8> pal;
	if (this->srcPF == Media::PF_B8G8R8A8)
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)this->srcWidth) x = (Int32)this->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)this->srcHeight) y = (Int32)this->srcHeight - 1;
		srcImgPtr += x * 4 + y * (OSInt)this->srcBpl;
		return ReadNUInt32(&srcImgPtr[0]);
	}
	else if (this->srcPF == Media::PF_PAL_8 && this->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)this->srcWidth) x = (Int32)this->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)this->srcHeight) y = (Int32)this->srcHeight - 1;
		srcImgPtr += x + y * (OSInt)this->srcBpl;
		return ReadNUInt32(&pal[srcImgPtr[0] * 4]);
	}
	else if (this->srcPF == Media::PF_PAL_4 && this->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)this->srcWidth) x = (Int32)this->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)this->srcHeight) y = (Int32)this->srcHeight - 1;
		srcImgPtr += (x >> 1) + y * (OSInt)this->srcBpl;
		UOSInt c;
		if (x & 1)
			c = srcImgPtr[0] & 15;
		else
			c = (srcImgPtr[0] >> 4) & 15;
		return ReadNUInt32(&pal[c * 4]);
	}
	else if (this->srcPF == Media::PF_PAL_1 && this->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)this->srcWidth) x = (Int32)this->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)this->srcHeight) y = (Int32)this->srcHeight - 1;
		srcImgPtr += (x >> 3) + y * (OSInt)this->srcBpl;
		UOSInt c = (srcImgPtr[0] >> (7 - (x & 7))) & 1;
		return ReadNUInt32(&pal[c * 4]);
	}
	else
	{
		printf("Unsupported PixelFormat: %s\r\n", Media::PixelFormatGetName(this->srcPF).v.Ptr());
		return 0;
	}
}

void Media::NearestNeighbourRemapper::DoRemap(UnsafeArray<const UInt8> srcImgPtr, UnsafeArray<UInt8> destImgPtr)
{
	Math::RectAreaDbl rect = this->destQuad.GetExterior();
	OSInt left = (OSInt)rect.min.x;
	OSInt top = (OSInt)rect.min.y;
	OSInt right = (OSInt)rect.max.x;
	OSInt bottom = (OSInt)rect.max.y;
	OSInt x;
	Double y;
	if (left < 0) left = 0;
	if (top < 0) top = 0;
	if (right >= (OSInt)this->destWidth) right = (OSInt)this->destWidth - 1;
	if (bottom >= (OSInt)this->destHeight) bottom = (OSInt)this->destHeight - 1;
	UOSInt destAdd = this->destBpl - (UOSInt)(right - left + 1) * 4;
	Math::Quadrilateral quad = this->destQuad;
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
	destImgPtr += (UOSInt)top * this->destBpl + (UOSInt)left * 4;
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
				UInt32 px = GetPixel32(srcImgPtr, coord);
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
					UInt32 px = GetPixel32(srcImgPtr, coord);
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
}

Media::NearestNeighbourRemapper::NearestNeighbourRemapper()
{
}

Media::NearestNeighbourRemapper::~NearestNeighbourRemapper()
{
}
