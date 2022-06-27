#include "Stdafx.h"
#include "Media/FBSurface.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <errno.h>

#ifndef FBIO_WAITFORVSYNC
  #define FBIO_WAITFORVSYNC _IOW('F', 0x20, __u32)
#endif

struct Media::FBSurface::ClassData
{
	MonitorHandle *hMon;
	Int32 fd;
	Int32 ttyfd;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	UInt8 *dataPtr;
	MonitorSurface *buffSurface;
	Bool bugHandleMode;
};

Bool Media::FBSurface::UpdateToScreen(Bool waitForVBlank)
{
	if (!this->clsData->bugHandleMode || this->clsData->ttyfd < 0)
	{
		return true;
	}
	if (waitForVBlank) this->WaitForVBlank();
	return write(this->clsData->ttyfd, " \r", 2) == 2;
}

Media::FBSurface::FBSurface(MonitorHandle *hMon, const Media::ColorProfile *color, Double dpi, Media::RotateType rotateType)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->hMon = hMon;
	this->clsData->buffSurface = 0;
	this->clsData->ttyfd = -1;
	this->clsData->bugHandleMode = false;
	Char sbuff[64];
	Text::StrUOSInt(Text::StrConcat(sbuff, "/dev/fb"), ((UOSInt)hMon) - 1);
	this->clsData->fd = open(sbuff, O_RDWR);
	if (this->clsData->fd < 0)
	{
		return;
	}
	this->clsData->ttyfd = open("/dev/tty1", O_WRONLY);
	if (this->clsData->ttyfd < 0)
	{
		printf("FBSurface: Error in opening tty %d\r\n", errno);
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

	this->info.fourcc = 0;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.atype = Media::AT_NO_ALPHA;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.storeBPP = this->clsData->vinfo.bits_per_pixel;
	this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
	this->info.dispWidth = this->clsData->vinfo.xres;
	this->info.dispHeight = this->clsData->vinfo.yres;
	this->info.storeWidth = (UOSInt)this->clsData->finfo.line_length / (this->info.storeBPP >> 3);
	this->info.storeHeight = this->info.dispHeight;
	this->info.byteSize = this->info.storeWidth * this->info.storeHeight * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = dpi;
	this->info.vdpi = dpi;
	this->info.rotateType = rotateType;
	this->info.color->Set(color);

	if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270)
	{
		UOSInt tmpV = this->info.dispWidth;
		this->info.dispWidth = this->info.dispHeight;
		this->info.dispHeight = tmpV;
	}
}

Media::FBSurface::~FBSurface()
{
	if (this->clsData->fd >= 0)
	{
		munmap(this->clsData->dataPtr, this->clsData->vinfo.yres * this->clsData->finfo.line_length);
		close(this->clsData->fd);
	}
	if (this->clsData->fd >= 0)
	{
		close(this->clsData->ttyfd);
	}
	MemFree(this->clsData);
}

Bool Media::FBSurface::IsError() const
{
	return this->clsData->fd < 0;
}

Media::Image *Media::FBSurface::Clone() const
{
	Media::FBSurface *surface;
	NEW_CLASS(surface, Media::FBSurface(this->clsData->hMon, this->info.color, this->info.hdpi, this->info.rotateType));
	return surface;
}

Media::Image::ImageType Media::FBSurface::GetImageType() const
{
	return Media::Image::IT_MONITORSURFACE;
}

void Media::FBSurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown) const
{
	OSInt right = left + (OSInt)width;
	OSInt bottom = top + (OSInt)height;
	if (left >= (OSInt)this->info.dispWidth || top >= (OSInt)this->info.dispHeight || right <= 0 || bottom <= 0)
	{
		return;
	}
	if ((UOSInt)right > this->info.dispWidth)
	{
		right = (OSInt)this->info.dispWidth;
	}
	if ((UOSInt)bottom > this->info.dispHeight)
	{
		bottom = (OSInt)this->info.dispHeight;
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
		ImageCopy_ImgCopyR(this->clsData->dataPtr + (left * (OSInt)(this->info.storeBPP >> 3)) + (top * (OSInt)this->clsData->finfo.line_length), destBuff, width * this->info.storeBPP >> 3, height, this->clsData->finfo.line_length, destBpl, upsideDown);
	}
}

void Media::FBSurface::WaitForVBlank()
{
	int arg = 0;
	ioctl(this->clsData->fd, FBIO_WAITFORVSYNC, &arg);
}

void *Media::FBSurface::GetHandle()
{
	return (void*)(OSInt)(1 + this->clsData->fd);
}

Bool Media::FBSurface::DrawFromBuff()
{
	if (this->clsData->buffSurface)
	{
		RotateType rt = Media::RotateTypeCalc(this->clsData->buffSurface->info.rotateType, this->info.rotateType);
		if (rt == Media::RotateType::None)
		{
			this->clsData->buffSurface->GetImageData(this->clsData->dataPtr, 0, 0, this->info.dispWidth, this->info.dispHeight, this->clsData->finfo.line_length, false);
		}
		else
		{
			OSInt lineAdd;
			UInt8 *buff = this->clsData->buffSurface->LockSurface(&lineAdd);
			if (buff == 0)
			{
				return false;
			}
			if (this->info.storeBPP == 32)
			{
				if (rt == Media::RotateType::CW_90)
				{
					ImageUtil_Rotate32_CW90(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
				else if (rt == Media::RotateType::CW_180)
				{
					ImageUtil_Rotate32_CW180(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
				else if (rt == Media::RotateType::CW_270)
				{
					ImageUtil_Rotate32_CW270(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
			}
			else if (this->info.storeBPP == 64)
			{
				if (rt == Media::RotateType::CW_90)
				{
					ImageUtil_Rotate64_CW90(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
				else if (rt == Media::RotateType::CW_180)
				{
					ImageUtil_Rotate64_CW180(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
				else if (rt == Media::RotateType::CW_270)
				{
					ImageUtil_Rotate64_CW270(buff, this->clsData->dataPtr, this->info.dispWidth, this->info.dispHeight, (UOSInt)lineAdd, this->clsData->finfo.line_length);
				}
			}
		}
		this->UpdateToScreen(true);
		return true;
	}
	return false;
}

Bool Media::FBSurface::DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank)
{
	if (surface && surface->info.dispWidth == this->info.dispWidth && surface->info.dispHeight == this->info.dispHeight && surface->info.storeBPP == this->info.storeBPP)
	{
		if (waitForVBlank && !this->clsData->bugHandleMode) this->WaitForVBlank();
		surface->GetImageData(this->clsData->dataPtr, 0, 0, this->info.dispWidth, this->info.dispHeight, this->clsData->finfo.line_length, false);
		this->UpdateToScreen(waitForVBlank);
		return true;
	}
	return false;
}

Bool Media::FBSurface::DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank)
{
	OSInt destWidth = (OSInt)this->info.dispWidth;
	OSInt destHeight = (OSInt)this->info.dispHeight;
	Bool succ = false;
	RotateType rt = this->info.rotateType;
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
	if (waitForVBlank && !this->clsData->bugHandleMode) this->WaitForVBlank();
	if ((OSInt)buffW > 0 && (OSInt)buffH > 0)
	{
		if (rt == Media::RotateType::None)
		{
			ImageCopy_ImgCopyR(buff + drawY * lineAdd + drawX * (OSInt)(this->info.storeBPP >> 3),
				(UInt8*)this->clsData->dataPtr + destY * (Int32)this->clsData->finfo.line_length + destX * ((OSInt)this->info.storeBPP >> 3),
				buffW * (this->info.storeBPP >> 3), buffH, (UOSInt)lineAdd, (UInt32)this->clsData->finfo.line_length, false);
		}
		else if (this->info.storeBPP == 32)
		{
			if (rt == Media::RotateType::CW_90)
			{
				ImageUtil_Rotate32_CW90(buff + drawY * lineAdd + drawX * 4, 
				this->clsData->dataPtr + destX * this->clsData->finfo.line_length + (OSInt)(this->info.dispHeight - (UOSInt)destY - buffH) * 4,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
			else if (rt == Media::RotateType::CW_180)
			{
				ImageUtil_Rotate32_CW180(buff + drawY * lineAdd + drawX * 4, 
				this->clsData->dataPtr + (OSInt)(this->info.dispHeight - (UOSInt)destY - buffH) * this->clsData->finfo.line_length + (OSInt)(this->info.dispWidth - (UOSInt)destX - buffW) * 4,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
			else if (rt == Media::RotateType::CW_270)
			{
				ImageUtil_Rotate32_CW270(buff + drawY * lineAdd + drawX * 4, 
				this->clsData->dataPtr + (OSInt)(this->info.dispWidth - (UOSInt)destX - buffW) * this->clsData->finfo.line_length + destY * 4,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
		}
		else if (this->info.storeBPP == 64)
		{
			if (rt == Media::RotateType::CW_90)
			{
				ImageUtil_Rotate64_CW90(buff + drawY * lineAdd + drawX * 8, 
				this->clsData->dataPtr + destX * (Int32)this->clsData->finfo.line_length + (OSInt)(this->info.dispHeight - (UOSInt)destY - buffH) * 8,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
			else if (rt == Media::RotateType::CW_180)
			{
				ImageUtil_Rotate64_CW180(buff + drawY * lineAdd + drawX * 8, 
				this->clsData->dataPtr + (OSInt)(this->info.dispHeight - (UOSInt)destY - buffH) * this->clsData->finfo.line_length + (OSInt)(this->info.dispWidth - (UOSInt)destX - buffW) * 8,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
			else if (rt == Media::RotateType::CW_270)
			{
				ImageUtil_Rotate64_CW270(buff + drawY * lineAdd + drawX * 8, 
				this->clsData->dataPtr + (OSInt)(this->info.dispWidth - (UOSInt)destX - buffW) * (Int32)this->clsData->finfo.line_length + destY * 8,
				buffW, buffH, (UOSInt)lineAdd, this->clsData->finfo.line_length);
			}
		}

		if (rt == Media::RotateType::None)
		{
			
		}
		else
		{
			OSInt oldX = destX;
			OSInt oldY = destY;
			UOSInt oldW = buffW;
			UOSInt oldH = buffH;
			if (rt == Media::RotateType::CW_90)
			{
				OSInt tmpV = destWidth;
				destWidth = destHeight;
				destHeight = tmpV;
				destX = destWidth - oldY - (OSInt)oldH;
				destY = oldX;
				buffW = oldH;
				buffH = oldW;
			}
			else if (rt == Media::RotateType::CW_180)
			{
				destX = destWidth - oldX - (OSInt)oldW;
				destY = destHeight - oldY - (OSInt)oldH;
			}
			else if (rt == Media::RotateType::CW_270)
			{
				OSInt tmpV = destWidth;
				destWidth = destHeight;
				destHeight = tmpV;
				destX = oldY;
				destY = destHeight - oldX - (OSInt)oldW;
				buffW = oldH;
				buffH = oldW;
			}
		}

		if (clearScn)
		{
			if (destY > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destY, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
			if (destY + (OSInt)buffH < (OSInt)destHeight)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + (destY + (OSInt)buffH) * this->clsData->finfo.line_length, (UOSInt)destWidth, (UOSInt)(destHeight - (OSInt)buffH - destY), (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
			if (destX > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * this->clsData->finfo.line_length, (UOSInt)destX, buffH, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
			if (destX + (OSInt)buffW < (OSInt)destWidth)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * this->clsData->finfo.line_length + (destX + (OSInt)buffW) * (OSInt)(this->info.storeBPP >> 3), (UOSInt)destWidth - (UOSInt)destX - buffW, buffH, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
		}
	}
	else if (clearScn)
	{
		if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270)
		{
			ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destHeight, (UOSInt)destWidth, (UInt32)this->clsData->finfo.line_length, 0xff000000);
		}
		else
		{
			ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destHeight, (UInt32)this->clsData->finfo.line_length, 0xff000000);
		}
	}
	this->UpdateToScreen(waitForVBlank);
	succ = true;
	return succ;
}

UInt8 *Media::FBSurface::LockSurface(OSInt *lineAdd)
{
	*lineAdd = this->clsData->finfo.line_length;
	return this->clsData->dataPtr;
}

void Media::FBSurface::UnlockSurface()
{
}

void Media::FBSurface::SetSurfaceBugMode(Bool surfaceBugMode)
{
	this->clsData->bugHandleMode = surfaceBugMode;
}

void Media::FBSurface::SetBuffSurface(Media::MonitorSurface *buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
