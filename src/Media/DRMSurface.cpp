#include "Stdafx.h"
#include "Media/DRMSurface.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <stdio.h>
#include <errno.h>

struct Media::DRMSurface::ClassData
{
	Int32 drmFd;
	MonitorHandle *hMon;
	UInt8 *dataPtr;
	UOSInt dataBpl;
	Media::MonitorSurface *buffSurface;
	drmModeModeInfo modeInfo;
	UInt32 fbId;
	UInt32 handle;
	drmModeCrtcPtr oldCtrc;
	UInt32 crtcId;
	UInt32 connId;
};

Media::DRMSurface::DRMSurface(Int32 fd, MonitorHandle *hMon, Media::ColorProfile *color, Double dpi)
{
	this->clsData = MemAlloc(ClassData, 1);
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
	OSInt monIndex = -1 + (OSInt)hMon;
	drmModeConnectorPtr connector;
	drmModeEncoderPtr enc;
	OSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = (UInt32)resources->count_connectors;
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
					this->info->fourcc = 0;
					this->info->ftype = Media::FT_NON_INTERLACE;
					this->info->atype = Media::AT_NO_ALPHA;
					this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
					this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					this->info->storeBPP = 32;
					this->info->pf = Media::PixelFormatGetDef(0, this->info->storeBPP);
					this->info->dispWidth = connector->modes[0].hdisplay;
					this->info->dispHeight = connector->modes[0].vdisplay;
					this->info->storeWidth = this->info->dispWidth; //(UOSInt)this->clsData->dataBpl / (this->info->storeBPP >> 3);
					this->info->storeHeight = this->info->dispHeight;
					this->info->byteSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
					this->info->par2 = 1.0;
					this->info->hdpi = dpi;
					this->info->vdpi = dpi;
					this->info->color->Set(color);

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
	if (this->info->dispWidth == 0 || this->clsData->crtcId == 0)
	{
		return;
	}

	drm_mode_create_dumb creq;
	drm_mode_map_dumb mreq;
	drm_mode_destroy_dumb dreq;

	MemClear(&creq, sizeof(drm_mode_create_dumb));
	creq.width = (UInt32)this->info->dispWidth;
	creq.height = (UInt32)this->info->dispHeight;
	creq.bpp = 32; // hard coding

	if (drmIoctl(this->clsData->drmFd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0)
	{
		return;
	}

	this->info->storeWidth = (UOSInt)creq.pitch / (this->info->storeBPP >> 3);
	this->info->byteSize = creq.size;
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

	this->clsData->dataPtr = (UInt8*)mmap64(0, this->info->byteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (__off64_t)mreq.offset);

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

		munmap(this->clsData->dataPtr, this->info->byteSize);

		drmModeRmFB(this->clsData->drmFd, this->clsData->fbId);
		MemClear(&dreq, sizeof(dreq));
		dreq.handle = this->clsData->handle;
		drmIoctl(this->clsData->drmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	}
	MemFree(this->clsData);
}

Bool Media::DRMSurface::IsError()
{
	return this->clsData->dataPtr == 0;
}

Media::Image *Media::DRMSurface::Clone()
{
	Media::DRMSurface *surface;
	NEW_CLASS(surface, Media::DRMSurface(this->clsData->drmFd, this->clsData->hMon, this->info->color, this->info->hdpi));
	return surface;
}

Media::Image::ImageType Media::DRMSurface::GetImageType()
{
	return Media::Image::IT_MONITORSURFACE;
}

void Media::DRMSurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
{
	if (this->clsData->dataPtr)
	{
		UOSInt buffBpl = this->info->storeWidth * (this->info->storeBPP >> 3);
		if (left == 0 && top == 0 && width == this->info->dispWidth && height == this->info->dispHeight && buffBpl == destBpl && !upsideDown)
		{
			MemCopyANC(destBuff, this->clsData->dataPtr, destBpl * height);
		}
		else
		{
			ImageCopy_ImgCopyR((UInt8*)this->clsData->dataPtr + top * (OSInt)buffBpl + left * (Int32)(this->info->storeBPP >> 3), destBuff, width * (this->info->storeBPP >> 3), height, buffBpl, destBpl, upsideDown);
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
		this->clsData->buffSurface->GetImageData(this->clsData->dataPtr, 0, 0, this->info->dispWidth, this->info->dispHeight, this->clsData->dataBpl, false);
		return true;
	}
	return false;
}

Bool Media::DRMSurface::DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank)
{
	if (surface && surface->info->dispWidth == this->info->dispWidth && surface->info->dispHeight == this->info->dispHeight && surface->info->storeBPP == this->info->storeBPP)
	{
		if (waitForVBlank) this->WaitForVBlank();
		surface->GetImageData(this->clsData->dataPtr, 0, 0, this->info->dispWidth, this->info->dispHeight, this->clsData->dataBpl, false);
		return true;
	}
	return false;
}

Bool Media::DRMSurface::DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank)
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
		ImageCopy_ImgCopyR(buff + drawY * lineAdd + drawX * (OSInt)(this->info->storeBPP >> 3),
			(UInt8*)this->clsData->dataPtr + destY * (OSInt)this->clsData->dataBpl + destX * ((OSInt)this->info->storeBPP >> 3),
			buffW * (this->info->storeBPP >> 3), buffH, (UOSInt)lineAdd, (UInt32)this->clsData->dataBpl, false);

		if (clearScn)
		{
			if (destY > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destY, (UInt32)this->clsData->dataBpl, 0);
			}
			if (destY + (OSInt)buffH < (OSInt)destHeight)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + (destY + (OSInt)buffH) * (OSInt)this->clsData->dataBpl, (UOSInt)destWidth, (UOSInt)(destHeight - (OSInt)buffH - destY), (UInt32)this->clsData->dataBpl, 0);
			}
			if (destX > 0)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * (OSInt)this->clsData->dataBpl, (UOSInt)destX, buffH, (UInt32)this->clsData->dataBpl, 0);
			}
			if (destX + (OSInt)buffW < (OSInt)destWidth)
			{
				ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr + destY * (OSInt)this->clsData->dataBpl + (destX + (OSInt)buffW) * (OSInt)(this->info->storeBPP >> 3), (UOSInt)destWidth - (UOSInt)destX - buffW, buffH, (UInt32)this->clsData->dataBpl, 0);
			}
		}
	}
	else if (clearScn)
	{
		ImageUtil_ImageColorFill32((UInt8*)this->clsData->dataPtr, (UOSInt)destWidth, (UOSInt)destHeight, (UInt32)this->clsData->dataBpl, 0);
	}
	succ = true;
	return succ;
}

UInt8 *Media::DRMSurface::LockSurface(OSInt *lineAdd)
{
	*lineAdd = (OSInt)this->clsData->dataBpl;
	return this->clsData->dataPtr;
}

void Media::DRMSurface::UnlockSurface()
{

}

void Media::DRMSurface::SetBuffSurface(Media::MonitorSurface *buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
