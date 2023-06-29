#include "Stdafx.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/MemorySurface.h"

Media::MemorySurface::MemorySurface(Math::Size2D<UOSInt> size, UOSInt bitPerPixel, const Media::ColorProfile *color, Double dpi)
{
	this->buffPtr = MemAllocA(UInt8, size.CalcArea() * (bitPerPixel >> 3));
	this->info.fourcc = 0;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.atype = Media::AT_NO_ALPHA;
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
	this->info.color->Set(color);
	this->info.rotateType = Media::RotateType::None;
}

Media::MemorySurface::~MemorySurface()
{
	MemFreeA(this->buffPtr);
}

Bool Media::MemorySurface::IsError() const
{
	return this->buffPtr == 0;
}

Media::Image *Media::MemorySurface::Clone() const
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(this->info.dispSize, this->info.storeBPP, this->info.color, this->info.hdpi));
	return surface;
}

Media::Image::ImageType Media::MemorySurface::GetImageType() const
{
	return Media::Image::ImageType::MonitorSurface;
}

void Media::MemorySurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	OSInt right = left + (OSInt)width;
	OSInt bottom = top + (OSInt)height;
	if (left >= (OSInt)this->info.dispSize.x || top >= (OSInt)this->info.dispSize.y || right <= 0 || bottom <= 0)
	{
		return;
	}
	if ((UOSInt)right > this->info.dispSize.x)
	{
		right = (OSInt)this->info.dispSize.x;
	}
	if ((UOSInt)bottom > this->info.dispSize.y)
	{
		bottom = (OSInt)this->info.dispSize.y;
	}
	if (upsideDown)
	{
		return;
	}
	else
	{
		if (left < 0)
		{
			destBuff += (-left) * (OSInt)(this->info.storeBPP >> 3);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (-top) * (OSInt)destBpl;
			top = 0;
		}
		width = (UOSInt)(right - left);
		height = (UOSInt)(bottom - top);
		Media::ImageUtil::ImageCopyR(destBuff, (OSInt)destBpl, this->buffPtr,
			(OSInt)this->GetDataBpl(), left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
	}
}

void Media::MemorySurface::WaitForVBlank()
{
}

void *Media::MemorySurface::GetHandle()
{
	return this->buffPtr;
}

Bool Media::MemorySurface::DrawFromBuff()
{
	return false;
}

Bool Media::MemorySurface::DrawFromSurface(Media::MonitorSurface *surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn, Bool waitForVBlank)
{
	if (surface && surface->info.storeBPP == this->info.storeBPP)
	{
		OSInt destWidth = (OSInt)this->info.dispSize.x;
		OSInt destHeight = (OSInt)this->info.dispSize.y;
		if (waitForVBlank) this->WaitForVBlank();
		OSInt drawX = 0;
		OSInt drawY = 0;
		if (destTL.x < 0)
		{
			drawX = -destTL.x;
			buffSize.x += (UOSInt)destTL.x;
			destTL.x = 0;
		}
		if (destTL.y < 0)
		{
			drawY = -destTL.y;
			buffSize.y += (UOSInt)destTL.y;
			destTL.y = 0;
		}
		if (destTL.x + (OSInt)buffSize.x > (OSInt)destWidth)
		{
			buffSize.x = (UOSInt)(destWidth - destTL.x);
		}
		if (destTL.y + (OSInt)buffSize.y > (OSInt)destHeight)
		{
			buffSize.y = (UOSInt)(destHeight - destTL.y);
		}
		if ((OSInt)buffSize.x > 0 && (OSInt)buffSize.y > 0)
		{
			if (this->info.atype == Media::AT_ALPHA && this->info.storeBPP == 32)
			{
				OSInt lineAdd;
				UInt8 *srcPtr = surface->LockSurface(&lineAdd);
				if (srcPtr)
				{
					ImageUtil_ConvR8G8B8N8_ARGB32(srcPtr + drawY * lineAdd + drawX * (OSInt)(this->info.storeBPP >> 3),
						(UInt8*)this->buffPtr + destTL.y * (OSInt)this->GetDataBpl() + destTL.x * ((OSInt)this->info.storeBPP >> 3),
						buffSize.x, buffSize.y, lineAdd, (OSInt)this->GetDataBpl());
					surface->UnlockSurface();
				}
			}
			else
			{
				surface->GetImageData((UInt8*)this->buffPtr + destTL.y * (OSInt)this->GetDataBpl() + destTL.x * ((OSInt)this->info.storeBPP >> 3),
					drawX, drawY, buffSize.x, buffSize.y, this->GetDataBpl(), false, this->info.rotateType);
			}

			if (clearScn)
			{
				UInt32 c = 0xFF000000;
				if (destTL.y > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->buffPtr, (UOSInt)destWidth, (UOSInt)destTL.y, this->GetDataBpl(), c);
				}
				if (destTL.y + (OSInt)buffSize.y < (OSInt)destHeight)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + ((UOSInt)destTL.y + buffSize.y) * this->GetDataBpl(), (UOSInt)destWidth, (UOSInt)(destHeight - (OSInt)buffSize.y - destTL.y), this->GetDataBpl(), c);
				}
				if (destTL.x > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + (UOSInt)destTL.y * this->GetDataBpl(), (UOSInt)destTL.x, buffSize.y, GetDataBpl(), c);
				}
				if (destTL.x + (OSInt)buffSize.x < (OSInt)destWidth)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + (UOSInt)destTL.y * this->GetDataBpl() + ((UOSInt)destTL.x + buffSize.x) * (this->info.storeBPP >> 3), (UOSInt)destWidth - (UOSInt)destTL.x - buffSize.x, buffSize.y, this->GetDataBpl(), c);
				}
			}
		}
		else if (clearScn)
		{
			ImageUtil_ImageColorFill32((UInt8*)this->buffPtr, (UOSInt)destWidth, (UOSInt)destHeight, this->GetDataBpl(), 0);
		}
		return true;
	}
	return false;
}

UInt8 *Media::MemorySurface::LockSurface(OSInt *lineAdd)
{
	*lineAdd = (OSInt)this->GetDataBpl();
	return this->buffPtr;
}

void Media::MemorySurface::UnlockSurface()
{

}

void Media::MemorySurface::SetSurfaceBugMode(Bool surfaceBugMode)
{

}