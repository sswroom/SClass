#include "Stdafx.h"
#include "Media/FBSurface.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "Sync/Thread.h"

struct Media::FBSurface::ClassData
{
	MonitorHandle *hMon;
	Int32 fd;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	UInt8 *dataPtr;
};

Media::FBSurface::FBSurface(MonitorHandle *hMon, Media::ColorProfile *color, Double dpi)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->hMon = hMon;
	Char sbuff[64];
	Text::StrUOSInt(Text::StrConcat(sbuff, "/dev/fb"), ((UOSInt)hMon) - 1);
	this->clsData->fd = open(sbuff, O_RDWR);
	if (this->clsData->fd < 0)
	{
		return;
	}
	ioctl(this->clsData->fd, FBIOGET_VSCREENINFO, &this->clsData->vinfo);
	ioctl(this->clsData->fd, FBIOGET_FSCREENINFO, &this->clsData->finfo);
	this->clsData->dataPtr = (UInt8*)mmap(0, this->clsData->vinfo.yres * this->clsData->finfo.line_length, PROT_READ | PROT_WRITE, MAP_SHARED, this->clsData->fd, (off_t)0);
	if (this->clsData->dataPtr == MAP_FAILED)
	{
		close(this->clsData->fd);
		this->clsData->fd = -1;
		return;
	}

	this->info->fourcc = 0;
	this->info->ftype = Media::FT_NON_INTERLACE;
	this->info->atype = Media::AT_NO_ALPHA;
	this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info->storeBPP = this->clsData->vinfo.bits_per_pixel;
	this->info->pf = Media::FrameInfo::GetDefPixelFormat(0, this->info->storeBPP);
	this->info->dispWidth = this->clsData->vinfo.xres;
	this->info->dispHeight = this->clsData->vinfo.yres;
	this->info->storeWidth = (UOSInt)this->clsData->finfo.line_length / (this->info->storeBPP >> 3);
	this->info->storeHeight = this->info->dispHeight;
	this->info->byteSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	this->info->par2 = 1.0;
	this->info->hdpi = dpi;
	this->info->vdpi = dpi;
	this->info->color->Set(color);
}

Media::FBSurface::~FBSurface()
{
	if (this->clsData->fd >= 0)
	{
		munmap(this->clsData->dataPtr, this->clsData->vinfo.yres * this->clsData->finfo.line_length);
		close(this->clsData->fd);
	}
	MemFree(this->clsData);
}

Bool Media::FBSurface::IsError()
{
	return this->clsData->fd < 0;
}

Media::Image *Media::FBSurface::Clone()
{
	Media::FBSurface *surface;
	NEW_CLASS(surface, Media::FBSurface(this->clsData->hMon, this->info->color, this->info->hdpi));
	return surface;
}

Media::Image::ImageType Media::FBSurface::GetImageType()
{
	return Media::Image::IT_MONITORSURFACE;
}

void Media::FBSurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
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
		ImageCopy_ImgCopyR(this->clsData->dataPtr + (left * (OSInt)(this->info->storeBPP >> 3)) + (top * (OSInt)this->clsData->finfo.line_length), destBuff, width * this->info->storeBPP >> 3, height, this->clsData->finfo.line_length, destBpl, upsideDown);
	}
	//////////////////////////
}
