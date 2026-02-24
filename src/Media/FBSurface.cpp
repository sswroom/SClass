#include "Stdafx.h"
#include "Media/FBSurface.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil.h"
#include "Media/ImageUtil_C.h"
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
	Optional<MonitorHandle> hMon;
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

Media::FBSurface::FBSurface(Optional<MonitorHandle> hMon, Optional<const Media::ColorProfile> color, Double dpi, Media::RotateType rotateType)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->hMon = hMon;
	this->clsData->buffSurface = nullptr;
	this->clsData->ttyfd = -1;
	this->clsData->bugHandleMode = false;
	Char sbuff[64];
	Text::StrUIntOS(Text::StrConcat(UARR(sbuff), "/dev/fb"), ((UIntOS)hMon.OrNull()) - 1);
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
	this->info.atype = Media::AT_IGNORE_ALPHA;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.storeBPP = this->clsData->vinfo.bits_per_pixel;
	this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
	this->info.dispSize.x = this->clsData->vinfo.xres;
	this->info.dispSize.y = this->clsData->vinfo.yres;
	this->info.storeSize.x = (UIntOS)this->clsData->finfo.line_length / (this->info.storeBPP >> 3);
	this->info.storeSize.y = this->info.dispSize.y;
	this->info.byteSize = this->info.storeSize.CalcArea() * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = dpi;
	this->info.vdpi = dpi;
	this->info.rotateType = rotateType;
	NN<const Media::ColorProfile> colornn;
	if (color.SetTo(colornn))
	{
		this->info.color.Set(colornn);
	}
	else
	{
		this->info.color.SetCommonProfile(Media::ColorProfile::CPT_VDISPLAY);
	}

	if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
	{
		UIntOS tmpV = this->info.dispSize.x;
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
	NEW_CLASSNN(surface, Media::FBSurface(this->clsData->hMon, this->info.color, this->info.hdpi, this->info.rotateType));
	return surface;
}

Media::RasterImage::ImageClass Media::FBSurface::GetImageClass() const
{
	return Media::RasterImage::ImageClass::MonitorSurface;
}

void Media::FBSurface::GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const
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
		Media::ImageUtil::ImageCopyR(destBuff, (IntOS)destBpl, this->clsData->dataPtr,
			(IntOS)this->GetDataBpl(), left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
	}
}

void Media::FBSurface::WaitForVBlank()
{
	int arg = 0;
	ioctl(this->clsData->fd, FBIO_WAITFORVSYNC, &arg);
}

void *Media::FBSurface::GetHandle()
{
	return (void*)(IntOS)(1 + this->clsData->fd);
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
			IntOS lineAdd;
			UnsafeArray<UInt8> buff;
			if (!buffSurface->LockSurface(lineAdd).SetTo(buff))
			{
				return false;
			}
			if (this->info.storeBPP == 32)
			{
				switch (rt)
				{
				case Media::RotateType::None:
					ImageCopy_ImgCopyR(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x * 4, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::CW_90:
					ImageUtil_Rotate32_CW90(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_180:
					ImageUtil_Rotate32_CW180(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_270:
					ImageUtil_Rotate32_CW270(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP:
					ImageUtil_HFlip32(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::HFLIP_CW_90:
					ImageUtil_HFRotate32_CW90(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_180:
					ImageUtil_HFRotate32_CW180(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_270:
					ImageUtil_HFRotate32_CW270(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				}
			}
			else if (this->info.storeBPP == 64)
			{
				switch (rt)
				{
				case Media::RotateType::None:
					ImageCopy_ImgCopyR(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x * 8, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::CW_90:
					ImageUtil_Rotate64_CW90(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_180:
					ImageUtil_Rotate64_CW180(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::CW_270:
					ImageUtil_Rotate64_CW270(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP:
					ImageUtil_HFlip64(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length, false);
					break;
				case Media::RotateType::HFLIP_CW_90:
					ImageUtil_HFRotate64_CW90(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_180:
					ImageUtil_HFRotate64_CW180(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				case Media::RotateType::HFLIP_CW_270:
					ImageUtil_HFRotate64_CW270(buff.Ptr(), this->clsData->dataPtr, this->info.dispSize.x, this->info.dispSize.y, (UIntOS)lineAdd, this->clsData->finfo.line_length);
					break;
				}
			}
		}
		this->UpdateToScreen(true);
		return true;
	}
	return false;
}

Bool Media::FBSurface::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank)
{
	if (surface->info.storeBPP == this->info.storeBPP)
	{
		IntOS destWidth = (IntOS)this->info.dispSize.x;
		IntOS destHeight = (IntOS)this->info.dispSize.y;
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
		if (destTL.x + (IntOS)buffSize.x > (IntOS)destWidth)
		{
			buffSize.x = (UIntOS)(destWidth - destTL.x);
		}
		if (destTL.y + (IntOS)buffSize.y > (IntOS)destHeight)
		{
			buffSize.y = (UIntOS)(destHeight - destTL.y);
		}
		if (waitForVBlank && !this->clsData->bugHandleMode) this->WaitForVBlank();
		if ((IntOS)buffSize.x > 0 && (IntOS)buffSize.y > 0)
		{
			if (this->info.rotateType == Media::RotateType::None || this->info.rotateType == Media::RotateType::HFLIP)
			{
				surface->GetRasterData(this->clsData->dataPtr + destTL.y * (Int32)this->clsData->finfo.line_length + destTL.x * ((IntOS)this->info.storeBPP >> 3),
					drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
			}
			else
			{
				IntOS oldX = destTL.x;
				IntOS oldY = destTL.y;
				UIntOS oldW = buffSize.x;
				UIntOS oldH = buffSize.y;
				if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_90)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.x * (Int32)this->clsData->finfo.line_length + destTL.y * ((IntOS)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					IntOS tmpV = destWidth;
					destWidth = destHeight;
					destHeight = tmpV;
					destTL.x = destWidth - oldY - (IntOS)oldH;
					destTL.y = oldX;
					buffSize.x = oldH;
					buffSize.y = oldW;
				}
				else if (this->info.rotateType == Media::RotateType::CW_180 || this->info.rotateType == Media::RotateType::HFLIP_CW_180)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.y * (Int32)this->clsData->finfo.line_length + destTL.x * ((IntOS)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					destTL.x = destWidth - oldX - (IntOS)oldW;
					destTL.y = destHeight - oldY - (IntOS)oldH;
				}
				else if (this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
				{
					surface->GetRasterData(this->clsData->dataPtr + destTL.x * (Int32)this->clsData->finfo.line_length + destTL.y * ((IntOS)this->info.storeBPP >> 3),
						drawX, drawY, buffSize.x, buffSize.y, this->clsData->finfo.line_length, false, this->info.rotateType);
					IntOS tmpV = destWidth;
					destWidth = destHeight;
					destHeight = tmpV;
					destTL.x = oldY;
					destTL.y = destHeight - oldX - (IntOS)oldW;
					buffSize.x = oldH;
					buffSize.y = oldW;
				}
			}

			if (clearScn)
			{
				if (destTL.y > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UIntOS)destWidth, (UIntOS)destTL.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.y + (IntOS)buffSize.y < (IntOS)destHeight)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + (destTL.y + (IntOS)buffSize.y) * (IntOS)this->clsData->finfo.line_length, (UIntOS)destWidth, (UIntOS)(destHeight - (IntOS)buffSize.y - destTL.y), (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.x > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destTL.y * (IntOS)this->clsData->finfo.line_length, (UIntOS)destTL.x, buffSize.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
				if (destTL.x + (IntOS)buffSize.x < (IntOS)destWidth)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destTL.y * (IntOS)this->clsData->finfo.line_length + (destTL.x + (IntOS)buffSize.x) * (IntOS)(this->info.storeBPP >> 3), (UIntOS)destWidth - (UIntOS)destTL.x - buffSize.x, buffSize.y, (UInt32)this->clsData->finfo.line_length, 0xff000000);
				}
			}
		}
		else if (clearScn)
		{
			if (this->info.rotateType == Media::RotateType::CW_90 || this->info.rotateType == Media::RotateType::CW_270 || this->info.rotateType == Media::RotateType::HFLIP_CW_90 || this->info.rotateType == Media::RotateType::HFLIP_CW_270)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UIntOS)destHeight, (UIntOS)destWidth, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
			else
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UIntOS)destWidth, (UIntOS)destHeight, (UInt32)this->clsData->finfo.line_length, 0xff000000);
			}
		}
		this->UpdateToScreen(waitForVBlank);
		return true;
	}
	return false;
}

UnsafeArrayOpt<UInt8> Media::FBSurface::LockSurface(OutParam<IntOS> lineAdd)
{
	lineAdd.Set((IntOS)this->clsData->finfo.line_length);
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
