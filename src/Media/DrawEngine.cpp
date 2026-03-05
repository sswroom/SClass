#include "Stdafx.h"
#include "Media/DrawEngine.h"
#include "Media/ImageUtil_C.h"

Bool Media::DrawImage::DrawStringHAlign(Math::Coord2DDbl tl, Double brx, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Text::HAlignment hAlign)
{
	Math::Size2DDbl sz;
	if (hAlign == Text::HAlignment::Right)
	{
		sz = this->GetTextSize(f, str);
		if (!sz.HasArea())
		{
			return this->DrawString(tl, str, f, b);
		}
		else
		{
			return this->DrawString(Math::Coord2DDbl(brx - sz.GetWidth(), tl.y), str, f, b);
		}
	}
	else if (hAlign == Text::HAlignment::Center)
	{
		sz = this->GetTextSize(f, str);
		if (!sz.HasArea())
		{
			return this->DrawString(tl, str, f, b);
		}
		else
		{
			return this->DrawString(Math::Coord2DDbl(tl.x + (brx - tl.x - sz.GetWidth()) * 0.5, tl.y), str, f, b);
		}
	}
	else
	{
		return this->DrawString(tl, str, f, b);
	}
}

Bool Media::DrawImage::PixelSupported() const
{
	return false;
}

UIntOS Media::DrawImage::PixelGetWidth() const
{
	return 0;
}

UIntOS Media::DrawImage::PixelGetHeight() const
{
	return 0;
}

Math::Size2D<UIntOS> Media::DrawImage::PixelGetSize() const
{
	return Math::Size2D<UIntOS>(this->PixelGetWidth(), this->PixelGetHeight());
}

UInt32 Media::DrawImage::PixelGetBitCount() const
{
	return 0;
}

/*UnsafeArrayOpt<UInt8> Media::DrawImage::PixelGetBits(OutParam<Bool> revOrder)
{
	return nullptr;
}*/

void Media::DrawImage::PixelGetBitsEnd(Bool modified)
{
}

UIntOS Media::DrawImage::PixelGetBpl() const
{
	return 0;
}

Media::PixelFormat Media::DrawImage::PixelGetFormat() const
{
	return Media::PF_UNKNOWN;
}

UInt32 Media::DrawImage::PixelGet32(IntOS x, IntOS y)
{
	UIntOS width = this->PixelGetWidth();
	if (x < 0 || (UIntOS)x >= width)
		return 0;
	UIntOS height = this->PixelGetHeight();
	if (y < 0 || (UIntOS)y >= height)
		return 0;
	Bool revOrder;
	UnsafeArray<UInt8> bmpBits;
	if (!this->PixelGetBits(revOrder).SetTo(bmpBits))
		return 0;
	if (revOrder)
	{
		y = (IntOS)height - y - 1;
	}
	UInt32 bitCount = this->PixelGetBitCount();
	if (bitCount == 32)
	{
		return *(UInt32*)(((y * (IntOS)width + x) * 4) + bmpBits.Ptr());
	}
	else if (bitCount == 24)
	{
		return 0xff000000 | *(UInt32*)(((y * (IntOS)width + x) * 3) + bmpBits.Ptr());
	}
	else
	{
		return 0;
	}
}

void Media::DrawImage::SetImageAlpha(UInt8 alpha)
{
	Bool revOrder;
	UnsafeArray<UInt8> bmpBits;
	if (this->PixelGetBitCount() == 32 && this->PixelGetBits(revOrder).SetTo(bmpBits) && this->PixelGetFormat() == Media::PF_B8G8R8A8)
	{
		ImageUtil_ImageFillAlpha32(bmpBits.Ptr(), this->PixelGetWidth(), this->PixelGetHeight(), this->PixelGetBpl(), alpha);
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
	
	if (this->PixelGetBitCount() == 32 && this->PixelGetFormat() == Media::PF_B8G8R8A8)
	{
		Bool revOrder;
		UnsafeArray<UInt8> bmpBits;
		if (this->PixelGetBits(revOrder).SetTo(bmpBits))
		{
			ImageUtil_ImageAlphaMul32(bmpBits.Ptr(), this->PixelGetWidth(), this->PixelGetHeight(), this->PixelGetBpl(), (UInt32)Double2Int32(val * 65536.0));
		}
	}
}
