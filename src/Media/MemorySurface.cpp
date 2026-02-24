#include "Stdafx.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil.h"
#include "Media/ImageUtil_C.h"
#include "Media/MemorySurface.h"

Media::MemorySurface::MemorySurface(Math::Size2D<UIntOS> size, UIntOS bitPerPixel, Optional<const Media::ColorProfile> color, Double dpi)
{
	this->buffPtr = MemAllocA(UInt8, size.CalcArea() * (bitPerPixel >> 3));
	this->info.fourcc = 0;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.atype = Media::AT_IGNORE_ALPHA;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.storeBPP = (UInt32)bitPerPixel;
	this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
	this->info.dispSize = size;
	this->info.storeSize = size;
	this->info.byteSize = this->info.storeSize.CalcArea() * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = dpi;
	this->info.vdpi = dpi;
	NN<const Media::ColorProfile> colornn;
	if (color.SetTo(colornn))
	{
		this->info.color.Set(colornn);
	}
	else
	{
		this->info.color.SetCommonProfile(Media::ColorProfile::CPT_VUNKNOWN);
	}
	this->info.rotateType = Media::RotateType::None;
}

Media::MemorySurface::~MemorySurface()
{
	MemFreeAArr(this->buffPtr);
}

Bool Media::MemorySurface::IsError() const
{
	return false;
}

NN<Media::RasterImage> Media::MemorySurface::Clone() const
{
	NN<Media::MemorySurface> surface;
	NEW_CLASSNN(surface, Media::MemorySurface(this->info.dispSize, this->info.storeBPP, &this->info.color, this->info.hdpi));
	return surface;
}

Media::RasterImage::ImageClass Media::MemorySurface::GetImageClass() const
{
	return Media::RasterImage::ImageClass::MonitorSurface;
}

void Media::MemorySurface::GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	IntOS right = left + (IntOS)width;
	IntOS bottom = top + (IntOS)height;
	if (left >= (IntOS)this->info.dispSize.x || top >= (IntOS)this->info.dispSize.y || right <= 0 || bottom <= 0)
	{
		return;
	}
	if ((UIntOS)right > this->info.dispSize.x)
	{
		right = (IntOS)this->info.dispSize.x;
	}
	if ((UIntOS)bottom > this->info.dispSize.y)
	{
		bottom = (IntOS)this->info.dispSize.y;
	}
	if (upsideDown)
	{
		return;
	}
	else
	{
		if (left < 0)
		{
			destBuff += (-left) * (IntOS)(this->info.storeBPP >> 3);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (-top) * (IntOS)destBpl;
			top = 0;
		}
		width = (UIntOS)(right - left);
		height = (UIntOS)(bottom - top);
		Media::ImageUtil::ImageCopyR(destBuff, (IntOS)destBpl, this->buffPtr,
			(IntOS)this->GetDataBpl(), left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
	}
}

void Media::MemorySurface::WaitForVBlank()
{
}

void *Media::MemorySurface::GetHandle()
{
	return this->buffPtr.Ptr();
}

Bool Media::MemorySurface::DrawFromBuff()
{
	return false;
}

Bool Media::MemorySurface::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank)
{
	if (surface->info.storeBPP == this->info.storeBPP)
	{
		IntOS destWidth = (IntOS)this->info.dispSize.x;
		IntOS destHeight = (IntOS)this->info.dispSize.y;
		Math::Size2D<UIntOS> dispSize = buffSize;
		Media::RotateType rt = Media::RotateTypeCalc(this->info.rotateType, surface->info.rotateType);
		if (waitForVBlank) this->WaitForVBlank();
		IntOS drawX = 0;
		IntOS drawY = 0;
		if (destTL.x < 0)
		{
			drawX = -destTL.x;
			buffSize.x += (UIntOS)destTL.x;
			destTL.x = 0;
		}
		if (destTL.y < 0)
		{
			drawY = -destTL.y;
			buffSize.y += (UIntOS)destTL.y;
			destTL.y = 0;
		}
		if (rt == Media::RotateType::CW_90 || rt == Media::RotateType::CW_270 || rt == Media::RotateType::HFLIP_CW_90 || rt == Media::RotateType::HFLIP_CW_270)
		{
			if (destTL.x + (IntOS)buffSize.x > (IntOS)this->info.dispSize.y)
			{
				buffSize.x = (UIntOS)((IntOS)this->info.dispSize.y - destTL.x);
			}
			if (destTL.y + (IntOS)buffSize.y > (IntOS)this->info.dispSize.x)
			{
				buffSize.y = (UIntOS)((IntOS)this->info.dispSize.x - destTL.y);
			}
			dispSize = buffSize.SwapXY();
			destTL = destTL.SwapXY();
			IntOS tmp = drawX;
			drawX = drawY;
			drawY = tmp;
		}
		else
		{
			if (destTL.x + (IntOS)buffSize.x > (IntOS)destWidth)
			{
				buffSize.x = (UIntOS)(destWidth - destTL.x);
			}
			if (destTL.y + (IntOS)buffSize.y > (IntOS)destHeight)
			{
				buffSize.y = (UIntOS)(destHeight - destTL.y);
			}
		}
		if ((IntOS)buffSize.x > 0 && (IntOS)buffSize.y > 0)
		{
			if (this->info.atype == Media::AT_ALPHA && this->info.storeBPP == 32)
			{
				if (surface->info.rotateType == this->info.rotateType)
				{
					IntOS lineAdd;
					UnsafeArray<UInt8> srcPtr;
					if (surface->LockSurface(lineAdd).SetTo(srcPtr))
					{
						ImageUtil_ConvR8G8B8N8_B8G8R8A8(srcPtr.Ptr() + drawY * lineAdd + drawX * (IntOS)(this->info.storeBPP >> 3),
							this->buffPtr.Ptr() + destTL.y * (IntOS)this->GetDataBpl() + destTL.x * ((IntOS)this->info.storeBPP >> 3),
							buffSize.x, buffSize.y, lineAdd, (IntOS)this->GetDataBpl());
						surface->UnlockSurface();
					}
				}
				else
				{
					UIntOS bpl = this->GetDataBpl();
					surface->GetRasterData(this->buffPtr.Ptr() + destTL.y * (IntOS)bpl + destTL.x * ((IntOS)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, bpl, false, this->info.rotateType);
					ImageUtil_ConvR8G8B8N8_B8G8R8A8(this->buffPtr.Ptr() + destTL.y * (IntOS)bpl + destTL.x * ((IntOS)this->info.storeBPP >> 3), this->buffPtr.Ptr() + destTL.y * (IntOS)bpl + destTL.x * ((IntOS)this->info.storeBPP >> 3), dispSize.x, dispSize.y, (IntOS)bpl, (IntOS)bpl);
				}
			}
			else
			{
				surface->GetRasterData(this->buffPtr + destTL.y * (IntOS)this->GetDataBpl() + destTL.x * ((IntOS)this->info.storeBPP >> 3),
					drawX, drawY, buffSize.x, buffSize.y, this->GetDataBpl(), false, this->info.rotateType);
			}

			if (clearScn)
			{
				UInt32 c = 0xFF000000;
				if (destTL.y > 0)
				{
					ImageUtil_ImageColorFill32(this->buffPtr.Ptr(), (UIntOS)destWidth, (UIntOS)destTL.y, this->GetDataBpl(), c);
				}
				if (destTL.y + (IntOS)dispSize.y < (IntOS)destHeight)
				{
					ImageUtil_ImageColorFill32(this->buffPtr.Ptr() + ((UIntOS)destTL.y + dispSize.y) * this->GetDataBpl(), (UIntOS)destWidth, (UIntOS)(destHeight - (IntOS)dispSize.y - destTL.y), this->GetDataBpl(), c);
				}
				if (destTL.x > 0)
				{
					ImageUtil_ImageColorFill32(this->buffPtr.Ptr() + (UIntOS)destTL.y * this->GetDataBpl(), (UIntOS)destTL.x, dispSize.y, GetDataBpl(), c);
				}
				if (destTL.x + (IntOS)dispSize.x < (IntOS)destWidth)
				{
					ImageUtil_ImageColorFill32(this->buffPtr.Ptr() + (UIntOS)destTL.y * this->GetDataBpl() + ((UIntOS)destTL.x + dispSize.x) * (this->info.storeBPP >> 3), (UIntOS)destWidth - (UIntOS)destTL.x - dispSize.x, dispSize.y, this->GetDataBpl(), c);
				}
			}
		}
		else if (clearScn)
		{
			ImageUtil_ImageColorFill32(this->buffPtr.Ptr(), (UIntOS)destWidth, (UIntOS)destHeight, this->GetDataBpl(), 0);
		}
		return true;
	}
	return false;
}

UnsafeArrayOpt<UInt8> Media::MemorySurface::LockSurface(OutParam<IntOS> lineAdd)
{
	lineAdd.Set((IntOS)this->GetDataBpl());
	return this->buffPtr;
}

void Media::MemorySurface::UnlockSurface()
{

}

void Media::MemorySurface::SetSurfaceBugMode(Bool surfaceBugMode)
{

}