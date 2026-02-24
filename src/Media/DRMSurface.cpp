#include "Stdafx.h"
#include "Media/DRMSurface.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil_C.h"
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_mode.h>

#include <stdio.h>
#include <errno.h>

struct Media::DRMSurface::ClassData
{
	Int32 drmFd;
	MonitorHandle *hMon;
	UInt8 *dataPtr;
	UIntOS dataBpl;
	Media::MonitorSurface *buffSurface;
	drmModeModeInfo modeInfo;
	UInt32 fbId;
	UInt32 handle;
	drmModeCrtcPtr oldCtrc;
	UInt32 crtcId;
	UInt32 connId;
};

Media::DRMSurface::DRMSurface(Int32 fd, MonitorHandle *hMon, NN<const Media::ColorProfile> color, Double dpi)
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->drmFd = fd;
	this->clsData->hMon = hMon;
	this->clsData->dataPtr = 0;
	this->clsData->dataBpl = 0;
	this->clsData->buffSurface = 0;
	this->clsData->oldCtrc = 0;
	this->clsData->crtcId = 0;
	this->clsData->connId = 0;

	drmModeResPtr resources = drmModeGetResources(this->clsData->drmFd);
	if (resources == 0)
	{
		return;
	}
	IntOS monIndex = -1 + (IntOS)hMon;
	drmModeConnectorPtr connector;
	drmModeEncoderPtr enc;
	IntOS cnt = 0;
	UIntOS i = 0;
	UIntOS j = (UInt32)resources->count_connectors;
	while (i < j)
	{
		connector = drmModeGetConnector(this->clsData->drmFd, resources->connectors[i]);
		if (connector)
		{
			if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0)
			{
				if (monIndex == cnt)
				{
					MemCopyNO(&this->clsData->modeInfo, &connector->modes[0], sizeof(this->clsData->modeInfo));
					this->info.fourcc = 0;
					this->info.ftype = Media::FT_NON_INTERLACE;
					this->info.atype = Media::AT_IGNORE_ALPHA;
					this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
					this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					this->info.storeBPP = 32;
					this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
					this->info.dispSize.x = connector->modes[0].hdisplay;
					this->info.dispSize.y = connector->modes[0].vdisplay;
					this->info.storeSize = this->info.dispSize; //(UIntOS)this->clsData->dataBpl / (this->info->storeBPP >> 3);
					this->info.byteSize = this->info.storeSize.x * this->info.storeSize.y * (this->info.storeBPP >> 3);
					this->info.par2 = 1.0;
					this->info.hdpi = dpi;
					this->info.vdpi = dpi;
					this->info.color.Set(color);

					this->clsData->connId = connector->connector_id;
					if ((enc = drmModeGetEncoder(fd, connector->encoder_id)) != 0)
					{
						this->clsData->crtcId = enc->crtc_id;
						drmModeFreeEncoder(enc);
					}
				}
				cnt++;
			}
			drmModeFreeConnector(connector);
		}
		i++;
	}
	drmModeFreeResources(resources);
	if (this->info.dispSize.x == 0 || this->clsData->crtcId == 0)
	{
		return;
	}

	drm_mode_create_dumb creq;
	drm_mode_map_dumb mreq;
	drm_mode_destroy_dumb dreq;

	MemClear(&creq, sizeof(drm_mode_create_dumb));
	creq.width = (UInt32)this->info.dispSize.x;
	creq.height = (UInt32)this->info.dispSize.y;
	creq.bpp = 32; // hard coding

	if (drmIoctl(this->clsData->drmFd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0)
	{
		return;
	}

	this->info.storeSize.x = (UIntOS)creq.pitch / (this->info.storeBPP >> 3);
	this->info.byteSize = creq.size;
	this->clsData->handle = creq.handle;

	if (drmModeAddFB(fd, creq.width, creq.height, 32, (UInt8)creq.bpp, creq.pitch, creq.handle, &this->clsData->fbId))
	{
		MemClear(&dreq, sizeof(dreq));
		dreq.handle = this->clsData->handle;
		drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	}

	MemClear(&mreq, sizeof(drm_mode_map_dumb));
	mreq.handle = creq.handle;

	if (drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq))
	{
		drmModeRmFB(fd, this->clsData->fbId);
		MemClear(&dreq, sizeof(dreq));
		dreq.handle = this->clsData->handle;
		drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		return;
	}

	this->clsData->dataPtr = (UInt8*)mmap64(0, this->info.byteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (__off64_t)mreq.offset);

	if (this->clsData->dataPtr == MAP_FAILED)
	{
		this->clsData->dataPtr = 0;
		drmModeRmFB(fd, this->clsData->fbId);
		MemClear(&dreq, sizeof(dreq));
		dreq.handle = this->clsData->handle;
		drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
		return;
	}
	this->clsData->oldCtrc = drmModeGetCrtc(fd, this->clsData->crtcId);
	if (drmModeSetCrtc(fd, this->clsData->crtcId, this->clsData->fbId, 0, 0, &this->clsData->connId, 1, &this->clsData->modeInfo))
	{
		printf("drmModeSetCrtc Failed: %x\r\n", errno);
	}
}

Media::DRMSurface::~DRMSurface()
{
	if (this->clsData->dataPtr)
	{
		drm_mode_destroy_dumb dreq;
		if (this->clsData->oldCtrc)
		{
			drmModeSetCrtc(this->clsData->drmFd, this->clsData->oldCtrc->crtc_id, this->clsData->oldCtrc->buffer_id,
				this->clsData->oldCtrc->x, this->clsData->oldCtrc->y, &this->clsData->connId, 1, &this->clsData->oldCtrc->mode);
		}
		drmModeFreeCrtc(this->clsData->oldCtrc);

		munmap(this->clsData->dataPtr, this->info.byteSize);

		drmModeRmFB(this->clsData->drmFd, this->clsData->fbId);
		MemClear(&dreq, sizeof(dreq));
		dreq.handle = this->clsData->handle;
		drmIoctl(this->clsData->drmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	}
	MemFreeNN(this->clsData);
}

Bool Media::DRMSurface::IsError()
{
	return this->clsData->dataPtr == 0;
}

NN<Media::RasterImage> Media::DRMSurface::Clone() const
{
	NN<Media::DRMSurface> surface;
	NEW_CLASSNN(surface, Media::DRMSurface(this->clsData->drmFd, this->clsData->hMon, this->info.color, this->info.hdpi));
	return surface;
}

Media::RasterImage::ImageClass Media::DRMSurface::GetImageClass() const
{
	return Media::RasterImage::ImageClass::MonitorSurface;
}

void Media::DRMSurface::GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	if (this->clsData->dataPtr)
	{
		UIntOS buffBpl = this->info.storeSize.x * (this->info.storeBPP >> 3);
		if (left == 0 && top == 0 && width == this->info.dispSize.x && height == this->info.dispSize.y && buffBpl == destBpl && !upsideDown)
		{
			MemCopyANC(destBuff.Ptr(), this->clsData->dataPtr, destBpl * height);
		}
		else
		{
			ImageCopy_ImgCopyR((UInt8*)this->clsData->dataPtr + top * (IntOS)buffBpl + left * (Int32)(this->info.storeBPP >> 3), destBuff.Ptr(), width * (this->info.storeBPP >> 3), height, buffBpl, destBpl, upsideDown);
		}
	}
}

void Media::DRMSurface::WaitForVBlank()
{

}

void *Media::DRMSurface::GetHandle()
{
	return this->clsData->dataPtr;
}

Bool Media::DRMSurface::DrawFromBuff()
{
	if (this->clsData->buffSurface)
	{
		this->clsData->buffSurface->GetRasterData(this->clsData->dataPtr, 0, 0, this->info.dispSize.x, this->info.dispSize.y, this->clsData->dataBpl, false, Media::RotateType::None);
		return true;
	}
	return false;
}

Bool Media::DRMSurface::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank)
{
	if (surface->info.storeBPP == this->info.storeBPP)
	{
		IntOS destWidth = (IntOS)this->info.dispSize.x;
		IntOS destHeight = (IntOS)this->info.dispSize.y;
		if (waitForVBlank) this->WaitForVBlank();
		IntOS drawX = 0;
		IntOS drawY = 0;
		IntOS destX = destTL.x;
		IntOS destY = destTL.y;
		UIntOS buffW = buffSize.x;
		UIntOS buffH = buffSize.y;
		if (destX < 0)
		{
			drawX = -destX;
			buffW += (UIntOS)destX;
			destX = 0;
		}
		if (destY < 0)
		{
			drawY = -destY;
			buffH += (UIntOS)destY;
			destY = 0;
		}
		if (destX + (IntOS)buffW > (IntOS)destWidth)
		{
			buffW = (UIntOS)(destWidth - destX);
		}
		if (destY + (IntOS)buffH > (IntOS)destHeight)
		{
			buffH = (UIntOS)(destHeight - destY);
		}
		if ((IntOS)buffW > 0 && (IntOS)buffH > 0)
		{
			surface->GetRasterData(this->clsData->dataPtr + destY * (Int32)this->clsData->finfo.line_length + destX * ((IntOS)this->info.storeBPP >> 3),
				drawX, drawY, buffW, buffH, this->clsData->finfo.line_length, false, Media::RotateType::None);

			if (clearScn)
			{
				if (destY > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UIntOS)destWidth, (UIntOS)destY, (UInt32)this->clsData->dataBpl, 0);
				}
				if (destY + (IntOS)buffH < (IntOS)destHeight)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + (destY + (IntOS)buffH) * (IntOS)this->clsData->dataBpl, (UIntOS)destWidth, (UIntOS)(destHeight - (IntOS)buffH - destY), (UInt32)this->clsData->dataBpl, 0);
				}
				if (destX > 0)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * (IntOS)this->clsData->dataBpl, (UIntOS)destX, buffH, (UInt32)this->clsData->dataBpl, 0);
				}
				if (destX + (IntOS)buffW < (IntOS)destWidth)
				{
					ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * (IntOS)this->clsData->dataBpl + (destX + (IntOS)buffW) * (IntOS)(this->info.storeBPP >> 3), (UIntOS)destWidth - (UIntOS)destX - buffW, buffH, (UInt32)this->clsData->dataBpl, 0);
				}
			}
		}
		else if (clearScn)
		{
			ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UIntOS)destWidth, (UIntOS)destHeight, (UInt32)this->clsData->dataBpl, 0);
		}
		return true;
	}
	return false;
}

UnsafeArrayOpt<UInt8> Media::DRMSurface::LockSurface(OutParam<IntOS> lineAdd)
{
	lineAdd.Set((IntOS)this->clsData->dataBpl);
	return this->clsData->dataPtr;
}

void Media::DRMSurface::UnlockSurface()
{

}

void Media::DRMSurface::SetSurfaceBugMode(Bool surfaceBugMode)
{

}

void Media::DRMSurface::SetBuffSurface(Media::MonitorSurface *buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
