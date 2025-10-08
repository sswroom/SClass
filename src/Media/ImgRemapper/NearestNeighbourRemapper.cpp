#include "Stdafx.h"
#include "Math/Triangle.h"
#include "Media/ImgRemapper/NearestNeighbourRemapper.h"

UInt32 __stdcall Media::ImgRemapper::NearestNeighbourRemapper::GetPixel32_B8G8R8A8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<NearestNeighbourRemapper> me = NN<NearestNeighbourRemapper>::ConvertFrom(self);
	Int32 x = Math_Round(srcCoord.x);
	Int32 y = Math_Round(srcCoord.y);
	if (x < 0) x = 0;
	if (x >= (OSInt)me->srcWidth) x = (Int32)me->srcWidth - 1;
	if (y < 0) y = 0;
	if (y >= (OSInt)me->srcHeight) y = (Int32)me->srcHeight - 1;
	srcImgPtr += x * 4 + y * (OSInt)me->srcBpl;
	return ReadNUInt32(&srcImgPtr[0]);
}

UInt32 __stdcall Media::ImgRemapper::NearestNeighbourRemapper::GetPixel32_PAL8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<NearestNeighbourRemapper> me = NN<NearestNeighbourRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)me->srcWidth) x = (Int32)me->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)me->srcHeight) y = (Int32)me->srcHeight - 1;
		srcImgPtr += x + y * (OSInt)me->srcBpl;
		return ReadNUInt32(&pal[srcImgPtr[0] * 4]);
	}
	return 0;
}

UInt32 __stdcall Media::ImgRemapper::NearestNeighbourRemapper::GetPixel32_PAL4(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<NearestNeighbourRemapper> me = NN<NearestNeighbourRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)me->srcWidth) x = (Int32)me->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)me->srcHeight) y = (Int32)me->srcHeight - 1;
		srcImgPtr += (x >> 1) + y * (OSInt)me->srcBpl;
		UOSInt c;
		if (x & 1)
			c = srcImgPtr[0] & 15;
		else
			c = (srcImgPtr[0] >> 4) & 15;
		return ReadNUInt32(&pal[c * 4]);
	}
	return 0;
}

UInt32 __stdcall Media::ImgRemapper::NearestNeighbourRemapper::GetPixel32_PAL1(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self)
{
	NN<NearestNeighbourRemapper> me = NN<NearestNeighbourRemapper>::ConvertFrom(self);
	UnsafeArray<const UInt8> pal;
	if (me->srcPal.SetTo(pal))
	{
		Int32 x = Math_Round(srcCoord.x);
		Int32 y = Math_Round(srcCoord.y);
		if (x < 0) x = 0;
		if (x >= (OSInt)me->srcWidth) x = (Int32)me->srcWidth - 1;
		if (y < 0) y = 0;
		if (y >= (OSInt)me->srcHeight) y = (Int32)me->srcHeight - 1;
		srcImgPtr += (x >> 3) + y * (OSInt)me->srcBpl;
		UOSInt c = (srcImgPtr[0] >> (7 - (x & 7))) & 1;
		return ReadNUInt32(&pal[c * 4]);
	}
	return 0;
}

Media::ImageRemapper::GetPixel32Func Media::ImgRemapper::NearestNeighbourRemapper::GetPixel32()
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
		printf("NearestNeighbourRemapper: Unsupported PixelFormat: %s\r\n", Media::PixelFormatGetName(this->srcPF).v.Ptr());
		return 0;
	}
}

Media::ImgRemapper::NearestNeighbourRemapper::NearestNeighbourRemapper()
{
}

Media::ImgRemapper::NearestNeighbourRemapper::~NearestNeighbourRemapper()
{
}
