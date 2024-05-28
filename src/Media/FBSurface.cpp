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
	Optional<MonitorSurface> buffSurface;
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
	this->info.dispSize.x = this->clsData->vinfo.xres;
	this->info.dispSize.y = this->clsData->vinfo.yres;
	this->info.storeSize.x = (UOSInt)this->clsData->finfo.line_length / (this->info.storeBPP >> 3);
	this->info.storeSize.y = this->info.dispSize.y;
	this->info.byteSize = this->info.storeSize.CalcArea() * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = dpi;
	this->info.vdpi = dpi;
	this->info.rotateType = rotateType;
	NN<const Media::ColorProfile> colornn;
	if (colornn.Set(color))
	{
		this->info.color.Set(colornn);
	}
	else
	{
		this->info.color.SetCommonProfile(Media::ColorProfile::CPT_VDISPLAY);
	}

	if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
	{
		UOSInt tmpV = this->info.dispSize.x;
		this->info.dispSize.x = this->info.dispSize.y;
		this->info.dispSize.y = tmpV;
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

NN<Media::RasterImage> Media::FBSurface::Clone() const
{
	NN<Media::FBSurface> surface;
	NEW_CLASSNN(surface, Media::FBSurface(this->clsData->hMon, &this->info.color, this->info.hdpi, this->info.rotateType));
	return surface;
}

Media::RasterImage::ImageType Media::FBSurface::GetImageType() const
{
	return Media::RasterImage::ImageType::MonitorSurface;
}

void Media::FBSurface::GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
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
		Media::ImageUtil::ImageCopyR(destBuff, (OSInt)destBpl, this->clsData->dataPtr,
			(OSInt)this->GetDataBpl(), left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
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
	NN<Media::MonitorSurface> buffSurface;
	if (this->clsData->buffSurface.SetTo(buffSurface))
	{
		RotateType rt = Media::RotateTypeCalc(buffSurface->info.rotateType, this->info.rotateType);
		if (rt == Media::RotateType::None)
		{
			buffSurface->GetRasterData(this->clsData->dataPtr, 0, 0, this->info.dispSize.x, this->info.dispSize.y, this->clsData->finfo.line_length, false, rt);
		}
		else
		{
			OSInt lineAdd;
			UInt8 *buff = buffSurface->LockSurface(lineAdd);
			if (buff == 0)
			{
				return false;
			}
			if (this->info.storeBPP == 32)
			{
				switch (rt)
				{
				case Media::RotateType::None:
					ImageCopy_ImgCopyR(buff, this->clsData->dataPtr, this->info.dispSize.x * 4, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::CW_90:
					ImageUtil_Rotate32_CW90(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_180:
					ImageUtil_Rotate32_CW180(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_270:
					ImageUtil_Rotate32_CW270(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP:
					ImageUtil_HFlip32(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::HFLIP_CW_90:
					ImageUtil_HFRotate32_CW90(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_180:
					ImageUtil_HFRotate32_CW180(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_270:
					ImageUtil_HFRotate32_CW270(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				}
			}
			else if (this->info.storeBPP == 64)
			{
				switch (rt)
				{
				case Media::RotateType::None:
					ImageCopy_ImgCopyR(buff, this->clsData->dataPtr, this->info.dispSize.x * 8, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::CW_90:
					ImageUtil_Rotate64_CW90(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_180:
					ImageUtil_Rotate64_CW180(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_270:
					ImageUtil_Rotate64_CW270(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP:
					ImageUtil_HFlip64(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::HFLIP_CW_90:
					ImageUtil_HFRotate64_CW90(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_180:
					ImageUtil_HFRotate64_CW180(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_270:
					ImageUtil_HFRotate64_CW270(buff, this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UOSInt)lineAdd, this->clsData->finfo.line_length);
					break;
				}
			}
		}
		this->UpdateToScreen(true);
		return true;
	}
	return false;
}

Bool Media::FBSurface::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn, Bool waitForVBlank)
{
	if (surface->info.storeBPP == this->info.storeBPP)
	{
		OSInt destWidth = (OSInt)this->info.dispSize.x;
		OSInt destHeight = (OSInt)this->info.dispSize.y;
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
		if (waitForVBlank && !this->clsData->bugHandleMode) this->WaitForVBlank();
		if ((OSInt)buffSize.x > 0 && (OSInt)buffSize.y > 0)
		{
			if (this->info.rotateType == Media::RotateType::None || this->info.rotateType == Media::RotateType::HFLIP)
			{
				surface->GetRasterData(this->clsData->dataPtr + destTL.y * (Int32)this->clsData->finfo.line_length + destTL.x * ((OSInt)this->info.storeBPP >> 3),
					drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
			}
			else
			{
				OSInt oldX = destTL.x;
				OSInt oldY = destTL.y;
				UOSInt oldW = buffSize.x;
				UOSInt oldH = buffSize.y;
				if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_90)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.x * (Int32)this->clsData->finfo.line_length + destTL.y * ((OSInt)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					OSInt tmpV = destWidth;
					destWidth = destHeight;
					destHeight = tmpV;
					destTL.x = destWidth - oldY - (OSInt)oldH;
					destTL.y = oldX;
					buffSize.x = oldH;
					buffSize.y = oldW;
				}
				else if (this->info.rotateType == Media::RotateType::CW_180 || this->info.rotateType == Media::RotateType::HFLIP_CW_180)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.y * (Int32)this->clsData->finfo.line_length + destTL.x * ((OSInt)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					destTL.x = destWidth - oldX - (OSInt)oldW;
					destTL.y = destHeight - oldY - (OSInt)oldH;
				}
				else if (this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.x * (Int32)this->clsData->finfo.line_length + destTL.y * ((OSInt)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					OSInt tmpV = destWidth;
					destWidth = destHeight;
					destHeight = tmpV;
					destTL.x = oldY;
					destTL.y = destHeight - oldX - (OSInt)oldW;
					buffSize.x = oldH;
					buffSize.y = oldW;
				}
			}

			if (clearScn)
			{
				if (destTL.y > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destTL.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.y + (OSInt)buffSize.y < (OSInt)destHeight)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + (destTL.y + (OSInt)buffSize.y) * (OSInt)this->clsData->finfo.line_length, (UOSInt)destWidth, (UOSInt)(destHeight - (OSInt)buffSize.y - destTL.y), (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.x > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destTL.y * (OSInt)this->clsData->finfo.line_length, (UOSInt)destTL.x, buffSize.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.x + (OSInt)buffSize.x < (OSInt)destWidth)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destTL.y * (OSInt)this->clsData->finfo.line_length + (destTL.x + (OSInt)buffSize.x) * (OSInt)(this->info.storeBPP >> 3), (UOSInt)destWidth - (UOSInt)destTL.x - buffSize.x, buffSize.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
			}
		}
		else if (clearScn)
		{
			if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destHeight, (UOSInt)destWidth, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
			else
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destHeight, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
		}
		this->UpdateToScreen(waitForVBlank);
		return true;
	}
	return false;
}

UInt8 *Media::FBSurface::LockSurface(OutParam<OSInt> lineAdd)
{
	lineAdd.Set((OSInt)this->clsData->finfo.line_length);
	return this->clsData->dataPtr;
}

void Media::FBSurface::UnlockSurface()
{
}

void Media::FBSurface::SetSurfaceBugMode(Bool surfaceBugMode)
{
	this->clsData->bugHandleMode = surfaceBugMode;
}

void Media::FBSurface::SetBuffSurface(NN<Media::MonitorSurface> buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
