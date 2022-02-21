#include "Stdafx.h"
#include "Media/DrawEngine.h"
#include "Media/ImageUtil.h"

UInt32 Media::DrawImage::GetPixel32(OSInt x, OSInt y)
{
	UOSInt width = this->GetWidth();
	if (x < 0 || (UOSInt)x >= width)
		return 0;
	UOSInt height = this->GetHeight();
	if (y < 0 || (UOSInt)y >= height)
		return 0;
	Bool revOrder;
	UInt8 *bmpBits = this->GetImgBits(&revOrder);
	if (bmpBits == 0)
		return 0;
	if (revOrder)
	{
		y = (OSInt)height - y;
	}
	UInt32 bitCount = this->GetBitCount();
	if (bitCount == 32)
	{
		return *(UInt32*)(((y * (OSInt)width + x) * 4) + bmpBits);
	}
	else if (bitCount == 24)
	{
		return 0xff000000 | *(UInt32*)(((y * (OSInt)width + x) * 3) + bmpBits);
	}
	else
	{
		return 0;
	}
}

void Media::DrawImage::SetImageAlpha(UInt8 alpha)
{
	Bool revOrder;
	UInt8 *bmpBits = this->GetImgBits(&revOrder);
	if (this->GetBitCount() == 32 && bmpBits && this->GetPixelFormat() == Media::PF_B8G8R8A8)
	{
		ImageUtil_ImageFillAlpha32(bmpBits, this->GetWidth(), this->GetHeight(), this->GetImgBpl(), alpha);
	}
}

void Media::DrawImage::MulImageAlpha(Double val)
{
	if (val >= 1.0)
		return;
	if (val <= 0.0)
	{
		SetImageAlpha(0);
		return;
	}
	
	if (this->GetBitCount() == 32 && this->GetPixelFormat() == Media::PF_B8G8R8A8)
	{
		Bool revOrder;
		UInt8 *bmpBits = this->GetImgBits(&revOrder);
		if (bmpBits)
		{
			ImageUtil_ImageAlphaMul32(bmpBits, this->GetWidth(), this->GetHeight(), this->GetImgBpl(), (UInt32)Double2Int32(val * 65536.0));

		}
	}
}
