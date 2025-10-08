#include "Stdafx.h"
#include "Math/Triangle.h"
#include "Media/ImgRemapper/LinearImageRemapper.h"

UInt32 __stdcall Media::ImgRemapper::LinearImageRemapper::GetPixel32_B8G8R8A8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<LinearImageRemapper> me = NN<LinearImageRemapper>::ConvertFrom(self);
	Bool xSingle = false;
	Bool ySingle = false;
	UInt32 c00;
	UInt32 c01;
	UInt32 c10;
	UInt32 c11;
	Int32 x = (Int32)srcCoord.x;
	Int32 y = (Int32)srcCoord.y;
	if (x < 0) { x = 0; srcCoord.x = 0; xSingle = true; }
	if (x >= (OSInt)me->srcWidth - 1) { x = (Int32)me->srcWidth - 1; srcCoord.x = x; xSingle = true; }
	if (y < 0) { y = 0; srcCoord.x = 0; ySingle = true; }
	if (y >= (OSInt)me->srcHeight - 1) { y = (Int32)me->srcHeight - 1; srcCoord.y = y; ySingle = true; }
	srcImgPtr += x * 4 + y * (OSInt)me->srcBpl;
	c00 = ReadNUInt32(&srcImgPtr[0]);
	if (xSingle)
	{
		c10 = c00;
	}
	else
	{
		c10 = ReadNUInt32(&srcImgPtr[4]);
	}
	if (ySingle)
	{
		c01 = c00;
		c11 = c10;
	}
	else
	{
		c01 = ReadNUInt32(&srcImgPtr[me->srcBpl]);
		if (xSingle)
			c11 = c01;
		else
			c11 = ReadNUInt32(&srcImgPtr[me->srcBpl + 4]);
	}
	Double xRate = srcCoord.x - x;
	Double xiRate = 1 - xRate;
	Double yRate = srcCoord.y - y;
	Double yiRate = 1 - yRate;
	Double c0;
	Double c1;
	UInt32 c;
	c0 = (c00 & 0xff) * xiRate + (c10 & 0xff) * xRate;
	c1 = (c01 & 0xff) * xiRate + (c11 & 0xff) * xRate;
	c = (UInt32)(Math_Round(c0 * yiRate + c1 * yRate));
	c0 = ((c00 & 0xff00) >> 8) * xiRate + ((c10 & 0xff00) >> 8) * xRate;
	c1 = ((c01 & 0xff00) >> 8) * xiRate + ((c11 & 0xff00) >> 8) * xRate;
	c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 8;
	c0 = ((c00 & 0xff0000) >> 16) * xiRate + ((c10 & 0xff0000) >> 16) * xRate;
	c1 = ((c01 & 0xff0000) >> 16) * xiRate + ((c11 & 0xff0000) >> 16) * xRate;
	c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 16;
	c0 = ((c00 & 0xff000000) >> 24) * xiRate + ((c10 & 0xff000000) >> 24) * xRate;
	c1 = ((c01 & 0xff000000) >> 24) * xiRate + ((c11 & 0xff000000) >> 24) * xRate;
	c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 24;
	return c;
}

UInt32 __stdcall Media::ImgRemapper::LinearImageRemapper::GetPixel32_PAL8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<LinearImageRemapper> me = NN<LinearImageRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Bool xSingle = false;
		Bool ySingle = false;
		UInt32 c00;
		UInt32 c01;
		UInt32 c10;
		UInt32 c11;
		Int32 x = (Int32)srcCoord.x;
		Int32 y = (Int32)srcCoord.y;
		if (x < 0) { x = 0; srcCoord.x = 0; xSingle = true; }
		if (x >= (OSInt)me->srcWidth - 1) { x = (Int32)me->srcWidth - 1; srcCoord.x = x; xSingle = true; }
		if (y < 0) { y = 0; srcCoord.x = 0; ySingle = true; }
		if (y >= (OSInt)me->srcHeight - 1) { y = (Int32)me->srcHeight - 1; srcCoord.y = y; ySingle = true; }
		srcImgPtr += x + y * (OSInt)me->srcBpl;
		c00 = ReadNUInt32(&pal[srcImgPtr[0] * 4]);
		if (xSingle)
		{
			c10 = c00;
		}
		else
		{
			c10 = ReadNUInt32(&pal[srcImgPtr[1] * 4]);
		}
		if (ySingle)
		{
			c01 = c00;
			c11 = c10;
		}
		else
		{
			c01 = ReadNUInt32(&pal[srcImgPtr[me->srcBpl] * 4]);
			if (xSingle)
				c11 = c01;
			else
				c11 = ReadNUInt32(&pal[srcImgPtr[me->srcBpl + 1] * 4]);;
		}
		Double xRate = srcCoord.x - x;
		Double xiRate = 1 - xRate;
		Double yRate = srcCoord.y - y;
		Double yiRate = 1 - yRate;
		Double c0;
		Double c1;
		UInt32 c;
		c0 = (c00 & 0xff) * xiRate + (c10 & 0xff) * xRate;
		c1 = (c01 & 0xff) * xiRate + (c11 & 0xff) * xRate;
		c = (UInt32)(Math_Round(c0 * yiRate + c1 * yRate));
		c0 = ((c00 & 0xff00) >> 8) * xiRate + ((c10 & 0xff00) >> 8) * xRate;
		c1 = ((c01 & 0xff00) >> 8) * xiRate + ((c11 & 0xff00) >> 8) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 8;
		c0 = ((c00 & 0xff0000) >> 16) * xiRate + ((c10 & 0xff0000) >> 16) * xRate;
		c1 = ((c01 & 0xff0000) >> 16) * xiRate + ((c11 & 0xff0000) >> 16) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 16;
		c0 = ((c00 & 0xff000000) >> 24) * xiRate + ((c10 & 0xff000000) >> 24) * xRate;
		c1 = ((c01 & 0xff000000) >> 24) * xiRate + ((c11 & 0xff000000) >> 24) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 24;
		return c;
	}
	return 0;
}

UInt32 __stdcall Media::ImgRemapper::LinearImageRemapper::GetPixel32_PAL4(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<LinearImageRemapper> me = NN<LinearImageRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Bool xSingle = false;
		Bool ySingle = false;
		UInt32 c00;
		UInt32 c01;
		UInt32 c10;
		UInt32 c11;
		Int32 x = (Int32)srcCoord.x;
		Int32 y = (Int32)srcCoord.y;
		if (x < 0) { x = 0; srcCoord.x = 0; xSingle = true; }
		if (x >= (OSInt)me->srcWidth - 1) { x = (Int32)me->srcWidth - 1; srcCoord.x = x; xSingle = true; }
		if (y < 0) { y = 0; srcCoord.x = 0; ySingle = true; }
		if (y >= (OSInt)me->srcHeight - 1) { y = (Int32)me->srcHeight - 1; srcCoord.y = y; ySingle = true; }
		srcImgPtr += (x >> 1) + y * (OSInt)me->srcBpl;
		if (x & 1)
		{
			c00 = ReadNUInt32(&pal[(srcImgPtr[0] & 15) * 4]);
			if (xSingle)
			{
				c10 = c00;
			}
			else
			{
				c10 = ReadNUInt32(&pal[(srcImgPtr[1] >> 4) * 4]);
			}
			if (ySingle)
			{
				c01 = c00;
				c11 = c10;
			}
			else
			{
				c01 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl] & 15) * 4]);
				if (xSingle)
					c11 = c01;
				else
					c11 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl + 1] >> 4) * 4]);;
			}
		}
		else
		{
			c00 = ReadNUInt32(&pal[(srcImgPtr[0] >> 4) * 4]);
			if (xSingle)
			{
				c10 = c00;
			}
			else
			{
				c10 = ReadNUInt32(&pal[(srcImgPtr[0] & 15) * 4]);
			}
			if (ySingle)
			{
				c01 = c00;
				c11 = c10;
			}
			else
			{
				c01 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl] >> 4) * 4]);
				if (xSingle)
					c11 = c01;
				else
					c11 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl] & 15) * 4]);
			}
		}
		Double xRate = srcCoord.x - x;
		Double xiRate = 1 - xRate;
		Double yRate = srcCoord.y - y;
		Double yiRate = 1 - yRate;
		Double c0;
		Double c1;
		UInt32 c;
		c0 = (c00 & 0xff) * xiRate + (c10 & 0xff) * xRate;
		c1 = (c01 & 0xff) * xiRate + (c11 & 0xff) * xRate;
		c = (UInt32)(Math_Round(c0 * yiRate + c1 * yRate));
		c0 = ((c00 & 0xff00) >> 8) * xiRate + ((c10 & 0xff00) >> 8) * xRate;
		c1 = ((c01 & 0xff00) >> 8) * xiRate + ((c11 & 0xff00) >> 8) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 8;
		c0 = ((c00 & 0xff0000) >> 16) * xiRate + ((c10 & 0xff0000) >> 16) * xRate;
		c1 = ((c01 & 0xff0000) >> 16) * xiRate + ((c11 & 0xff0000) >> 16) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 16;
		c0 = ((c00 & 0xff000000) >> 24) * xiRate + ((c10 & 0xff000000) >> 24) * xRate;
		c1 = ((c01 & 0xff000000) >> 24) * xiRate + ((c11 & 0xff000000) >> 24) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 24;
		return c;
	}
	return 0;
}

UInt32 __stdcall Media::ImgRemapper::LinearImageRemapper::GetPixel32_PAL1(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<LinearImageRemapper> me = NN<LinearImageRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Bool xSingle = false;
		Bool ySingle = false;
		UInt32 c00;
		UInt32 c01;
		UInt32 c10;
		UInt32 c11;
		UOSInt i;
		Int32 x = (Int32)srcCoord.x;
		Int32 y = (Int32)srcCoord.y;
		if (x < 0) { x = 0; srcCoord.x = 0; xSingle = true; }
		if (x >= (OSInt)me->srcWidth - 1) { x = (Int32)me->srcWidth - 1; srcCoord.x = x; xSingle = true; }
		if (y < 0) { y = 0; srcCoord.x = 0; ySingle = true; }
		if (y >= (OSInt)me->srcHeight - 1) { y = (Int32)me->srcHeight - 1; srcCoord.y = y; ySingle = true; }
		srcImgPtr += (x >> 3) + y * (OSInt)me->srcBpl;
		if ((x & 7) == 7)
		{
			i = (srcImgPtr[0] >> (7 - (x & 7))) & 1;
			c00 = ReadNUInt32(&pal[i * 4]);
			if (xSingle)
			{
				c10 = c00;
			}
			else
			{
				c10 = ReadNUInt32(&pal[(srcImgPtr[1] >> 4) * 4]);
			}
			if (ySingle)
			{
				c01 = c00;
				c11 = c10;
			}
			else
			{
				c01 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl] & 15) * 4]);
				if (xSingle)
					c11 = c01;
				else
					c11 = ReadNUInt32(&pal[(srcImgPtr[me->srcBpl + 1] >> 4) * 4]);;
			}
		}
		else
		{
			i = (srcImgPtr[0] >> (7 - (x & 7))) & 1;
			c00 = ReadNUInt32(&pal[i * 4]);
			if (xSingle)
			{
				c10 = c00;
			}
			else
			{
				i = (srcImgPtr[0] >> (7 - ((x + 1) & 7))) & 1;
				c10 = ReadNUInt32(&pal[i * 4]);
			}
			if (ySingle)
			{
				c01 = c00;
				c11 = c10;
			}
			else
			{
				i = (srcImgPtr[me->srcBpl] >> (7 - (x & 7))) & 1;
				c01 = ReadNUInt32(&pal[i * 4]);
				if (xSingle)
					c11 = c01;
				else
				{
					i = (srcImgPtr[me->srcBpl] >> (7 - ((x + 1) & 7))) & 1;
					c11 = ReadNUInt32(&pal[i * 4]);;
				}
			}
		}
		Double xRate = srcCoord.x - x;
		Double xiRate = 1 - xRate;
		Double yRate = srcCoord.y - y;
		Double yiRate = 1 - yRate;
		Double c0;
		Double c1;
		UInt32 c;
		c0 = (c00 & 0xff) * xiRate + (c10 & 0xff) * xRate;
		c1 = (c01 & 0xff) * xiRate + (c11 & 0xff) * xRate;
		c = (UInt32)(Math_Round(c0 * yiRate + c1 * yRate));
		c0 = ((c00 & 0xff00) >> 8) * xiRate + ((c10 & 0xff00) >> 8) * xRate;
		c1 = ((c01 & 0xff00) >> 8) * xiRate + ((c11 & 0xff00) >> 8) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 8;
		c0 = ((c00 & 0xff0000) >> 16) * xiRate + ((c10 & 0xff0000) >> 16) * xRate;
		c1 = ((c01 & 0xff0000) >> 16) * xiRate + ((c11 & 0xff0000) >> 16) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 16;
		c0 = ((c00 & 0xff000000) >> 24) * xiRate + ((c10 & 0xff000000) >> 24) * xRate;
		c1 = ((c01 & 0xff000000) >> 24) * xiRate + ((c11 & 0xff000000) >> 24) * xRate;
		c |= (UInt32)(Math_Round(c0 * yiRate + c1 * yRate)) << 24;
		return c;
	}
	return 0;
}

Media::ImageRemapper::GetPixel32Func Media::ImgRemapper::LinearImageRemapper::GetPixel32()
{
	UnsafeArray<const UInt8> pal;
	if (this->srcPF == Media::PF_B8G8R8A8)
	{
		return GetPixel32_B8G8R8A8;
	}
	else if ((this->srcPF == Media::PF_PAL_8 || this->srcPF == Media::PF_PAL_W8) && this->srcPal.SetTo(pal))
	{
		return GetPixel32_PAL8;
	}
	else if (this->srcPF == Media::PF_PAL_4 && this->srcPal.SetTo(pal))
	{
		return GetPixel32_PAL4;
	}
	else if (this->srcPF == Media::PF_PAL_1 && this->srcPal.SetTo(pal))
	{
		return GetPixel32_PAL1;
	}
	else
	{
		printf("LinearImageRemapper: Unsupported PixelFormat: %s\r\n", Media::PixelFormatGetName(this->srcPF).v.Ptr());
		return 0;
	}
}

Media::ImgRemapper::LinearImageRemapper::LinearImageRemapper()
{
}

Media::ImgRemapper::LinearImageRemapper::~LinearImageRemapper()
{
}
