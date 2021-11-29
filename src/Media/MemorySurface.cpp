#include "Stdafx.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/MemorySurface.h"

Media::MemorySurface::MemorySurface(UOSInt width, UOSInt height, UOSInt bitPerPixel, Media::ColorProfile *color, Double dpi)
{
	this->buffPtr = MemAllocA(UInt8, width * height * (bitPerPixel >> 3));
	this->info->fourcc = 0;
	this->info->ftype = Media::FT_NON_INTERLACE;
	this->info->atype = Media::AT_NO_ALPHA;
	this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info->storeBPP = (UInt32)bitPerPixel;
	this->info->pf = Media::PixelFormatGetDef(0, this->info->storeBPP);
	this->info->dispWidth = width;
	this->info->dispHeight = height;
	this->info->storeWidth = width;
	this->info->storeHeight = height;
	this->info->byteSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	this->info->par2 = 1.0;
	this->info->hdpi = dpi;
	this->info->vdpi = dpi;
	this->info->color->Set(color);
	this->info->rotateType = Media::RotateType::None;
}

Media::MemorySurface::~MemorySurface()
{
	MemFreeA(this->buffPtr);
}

Bool Media::MemorySurface::IsError()
{
	return this->buffPtr == 0;
}

Media::Image *Media::MemorySurface::Clone()
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(this->info->dispWidth, this->info->dispHeight, this->info->storeBPP, this->info->color, this->info->hdpi));
	return surface;
}

Media::Image::ImageType Media::MemorySurface::GetImageType()
{
	return Media::Image::IT_MONITORSURFACE;
}

void Media::MemorySurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
{
	OSInt right = left + (OSInt)width;
	OSInt bottom = top + (OSInt)height;
	if (left >= (OSInt)this->info->dispWidth || top >= (OSInt)this->info->dispHeight || right <= 0 || bottom <= 0)
	{
		return;
	}
	if ((UOSInt)right > this->info->dispWidth)
	{
		right = (OSInt)this->info->dispWidth;
	}
	if ((UOSInt)bottom > this->info->dispHeight)
	{
		bottom = (OSInt)this->info->dispHeight;
	}
	if (upsideDown)
	{
		return;
	}
	else
	{
		if (left < 0)
		{
			destBuff += (-left) * (OSInt)(this->info->storeBPP >> 3);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (-top) * (OSInt)destBpl;
			top = 0;
		}
		width = (UOSInt)(right - left);
		height = (UOSInt)(bottom - top);
		ImageCopy_ImgCopyR(this->buffPtr + (left * (OSInt)(this->info->storeBPP >> 3)) + (top * (OSInt)this->GetDataBpl()), destBuff, width * this->info->storeBPP >> 3, height, this->GetDataBpl(), destBpl, upsideDown);
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

Bool Media::MemorySurface::DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank)
{
	if (surface && surface->info->dispWidth == this->info->dispWidth && surface->info->dispHeight == this->info->dispHeight && surface->info->storeBPP == this->info->storeBPP)
	{
		if (waitForVBlank) this->WaitForVBlank();
		if (this->info->atype == Media::AT_ALPHA && surface->info->atype == Media::AT_NO_ALPHA)
		{
			OSInt lineAdd;
			UInt8 *srcPtr = surface->LockSurface(&lineAdd);
			if (srcPtr)
			{
				ImageUtil_ConvR8G8B8N8_ARGB32(srcPtr, this->buffPtr, this->info->dispWidth, this->info->dispHeight, lineAdd, (OSInt)this->GetDataBpl());
				surface->UnlockSurface();
			}
		}
		else
		{
			surface->GetImageData(this->buffPtr, 0, 0, this->info->dispWidth, this->info->dispHeight, this->GetDataBpl(), false);
		}
		return true;
	}
	return false;
}

Bool Media::MemorySurface::DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank)
{
	OSInt destWidth = (OSInt)this->info->dispWidth;
	OSInt destHeight = (OSInt)this->info->dispHeight;
	Bool succ = false;
	if (waitForVBlank) this->WaitForVBlank();
	OSInt drawX = 0;
	OSInt drawY = 0;
	if (destX < 0)
	{
		drawX = -destX;
		buffW += (UOSInt)destX;
		destX = 0;
	}
	if (destY < 0)
	{
		drawY = -destY;
		buffH += (UOSInt)destY;
		destY = 0;
	}
	if (destX + (OSInt)buffW > (OSInt)destWidth)
	{
		buffW = (UOSInt)(destWidth - destX);
	}
	if (destY + (OSInt)buffH > (OSInt)destHeight)
	{
		buffH = (UOSInt)(destHeight - destY);
	}
	if ((OSInt)buffW > 0 && (OSInt)buffH > 0)
	{
		if (this->info->atype == Media::AT_ALPHA && this->info->storeBPP == 32)
		{
			ImageUtil_ConvR8G8B8N8_ARGB32(buff + drawY * lineAdd + drawX * (OSInt)(this->info->storeBPP >> 3),
				(UInt8*)this->buffPtr + destY * (OSInt)this->GetDataBpl() + destX * ((OSInt)this->info->storeBPP >> 3),
				buffW, buffH, lineAdd, (OSInt)this->GetDataBpl());
		}
		else
		{
			ImageCopy_ImgCopyR(buff + drawY * lineAdd + drawX * (OSInt)(this->info->storeBPP >> 3),
				(UInt8*)this->buffPtr + destY * (OSInt)this->GetDataBpl() + destX * ((OSInt)this->info->storeBPP >> 3),
				buffW * (this->info->storeBPP >> 3), buffH, (UOSInt)lineAdd, this->GetDataBpl(), false);
		}

		if (clearScn)
		{
			UInt32 c = 0xFF000000;
			if (destY > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->buffPtr, (UOSInt)destWidth, (UOSInt)destY, this->GetDataBpl(), c);
			}
			if (destY + (OSInt)buffH < (OSInt)destHeight)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + ((UOSInt)destY + buffH) * this->GetDataBpl(), (UOSInt)destWidth, (UOSInt)(destHeight - (OSInt)buffH - destY), this->GetDataBpl(), c);
			}
			if (destX > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + (UOSInt)destY * this->GetDataBpl(), (UOSInt)destX, buffH, GetDataBpl(), c);
			}
			if (destX + (OSInt)buffW < (OSInt)destWidth)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->buffPtr + (UOSInt)destY * this->GetDataBpl() + ((UOSInt)destX + buffW) * (this->info->storeBPP >> 3), (UOSInt)destWidth - (UOSInt)destX - buffW, buffH, this->GetDataBpl(), c);
			}
		}
	}
	else if (clearScn)
	{
		ImageUtil_ImageColorFill32((UInt8*)this->buffPtr, (UOSInt)destWidth, (UOSInt)destHeight, this->GetDataBpl(), 0);
	}
	succ = true;
	return succ;
}

UInt8 *Media::MemorySurface::LockSurface(OSInt *lineAdd)
{
	*lineAdd = (OSInt)this->GetDataBpl();
	return this->buffPtr;
}
void Media::MemorySurface::UnlockSurface()
{

}
