#include "Stdafx.h"
#include "Media/ImageCopyC.h"
#include "Media/MemorySurface.h"

Media::MemorySurface::MemorySurface(UOSInt width, UOSInt height, UOSInt bitPerPixel, Media::ColorProfile *color, Double dpi)
{
	this->buffPtr = MemAllocA(UInt8, width * height * (bitPerPixel >> 3));
	this->info->fourcc = 0;
	this->info->ftype = Media::FT_NON_INTERLACE;
	this->info->atype = Media::AT_NO_ALPHA;
	this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info->storeBPP = bitPerPixel;
	this->info->pf = Media::FrameInfo::GetDefPixelFormat(0, this->info->storeBPP);
	this->info->dispWidth = width;
	this->info->dispHeight = height;
	this->info->storeWidth = width;
	this->info->storeHeight = height;
	this->info->byteSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	this->info->par2 = 1.0;
	this->info->hdpi = dpi;
	this->info->vdpi = dpi;
	this->info->color->Set(color);
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
