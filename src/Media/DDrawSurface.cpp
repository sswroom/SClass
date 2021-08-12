#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/DDrawSurface.h"
#include "Media/ImageCopyC.h"
#include <windows.h>
#include <ddraw.h>

struct Media::DDrawSurface::ClassData
{
	Media::DDrawManager *mgr;
	LPDIRECTDRAW7 lpDD;
	LPDIRECTDRAWSURFACE7 surface;
	MonitorHandle *hMon;
	LPDIRECTDRAWCLIPPER clipper;
	Bool needRelease;
	Media::DDrawSurface *buffSurface;
};

Media::DDrawSurface::DDrawSurface(DDrawManager *mgr, void *lpDD, void *surface, MonitorHandle *hMon, Bool needRelease)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->mgr = mgr;
	this->clsData->lpDD = (LPDIRECTDRAW7)lpDD;
	this->clsData->surface = (LPDIRECTDRAWSURFACE7)surface;
	this->clsData->hMon = hMon;
	this->clsData->clipper = 0;
	this->clsData->needRelease = needRelease;
	this->clsData->buffSurface = 0;

	DDSURFACEDESC2 ddsd;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	this->clsData->surface->GetSurfaceDesc(&ddsd);

	this->info->fourcc = 0;
	this->info->ftype = Media::FT_NON_INTERLACE;
	this->info->atype = Media::AT_NO_ALPHA;
	this->info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info->storeBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
	this->info->pf = Media::FrameInfo::GetDefPixelFormat(0, this->info->storeBPP);
	if (this->info->storeBPP == 32 && mgr->Is10BitColor(hMon))
	{
		this->info->pf = Media::PF_LE_A2B10G10R10;
	}
	this->info->dispWidth = ddsd.dwWidth;
	this->info->dispHeight = ddsd.dwHeight;
	this->info->storeWidth = (UOSInt)ddsd.lPitch / (this->info->storeBPP >> 3);
	this->info->storeHeight = ddsd.dwHeight;
	this->info->byteSize = this->info->storeWidth * this->info->storeHeight * (this->info->storeBPP >> 3);
	this->info->par2 = 1.0;
	this->info->hdpi = mgr->GetMonitorDPI(hMon);;
	this->info->vdpi = this->info->hdpi;
	this->info->color->Set(mgr->GetMonProfile(hMon));
}

Media::DDrawSurface::~DDrawSurface()
{
	if (this->clsData->needRelease)
	{
		this->clsData->surface->Release();
	}
	if (this->clsData->clipper)
	{
		this->clsData->clipper->Release();
	}
	MemFree(this->clsData);
}

Media::Image *Media::DDrawSurface::Clone()
{
	return 0;
}

Media::Image::ImageType Media::DDrawSurface::GetImageType()
{
	return IT_MONITORSURFACE;
}

void Media::DDrawSurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
{
	HRESULT res;
	DDSURFACEDESC2 ddsd;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	res = this->clsData->surface->Lock(0, &ddsd, DDLOCK_WAIT, 0);
	if (res == DDERR_SURFACELOST)
	{
		this->clsData->surface->Restore();
		res = this->clsData->surface->Lock(0, &ddsd, DDLOCK_WAIT, 0);
	}
	if (res == DD_OK)
	{
		if (left == 0 && top == 0 && width == ddsd.dwWidth && height == ddsd.dwHeight && ddsd.lPitch == (OSInt)destBpl && !upsideDown && ddsd.dwWidth * (ddsd.ddpfPixelFormat.dwRGBBitCount >> 3) == destBpl)
		{
			MemCopyANC(destBuff, ddsd.lpSurface, destBpl * height);
		}
		else
		{
			ImageCopy_ImgCopyR((UInt8*)ddsd.lpSurface + top * ddsd.lPitch + left * (Int32)(ddsd.ddpfPixelFormat.dwRGBBitCount >> 3), destBuff, width, height, (UOSInt)(OSInt)ddsd.lPitch, destBpl, upsideDown);
		}
		this->clsData->surface->Unlock(0);
	}
}

void Media::DDrawSurface::WaitForVBlank()
{
	if (this->clsData->lpDD)
	{
		this->clsData->lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
	}
}

void *Media::DDrawSurface::GetHandle()
{
	return this->clsData->surface;
}

Bool Media::DDrawSurface::DrawFromBuff()
{
	if (this->clsData->buffSurface == 0)
	{
		return false;
	}
	HRESULT hRes = this->clsData->surface->Flip(0, 0);
	if (hRes == DDERR_SURFACELOST)
	{
		this->clsData->surface->Restore();
		((LPDIRECTDRAWSURFACE7)this->clsData->buffSurface->GetHandle())->Restore();
	}
	return hRes == DD_OK;
}

void Media::DDrawSurface::SetClipWindow(ControlHandle *clipWindow)
{
	if (this->clsData->clipper)
	{
		this->clsData->clipper->Release();
		this->clsData->clipper = 0;
	}

	this->clsData->lpDD->CreateClipper(0, &this->clsData->clipper, NULL);
	if (this->clsData->clipper->SetHWnd(0, (HWND)clipWindow) != DD_OK)
	{
		this->clsData->clipper->Release();
		this->clsData->clipper = 0;
	}
	else
	{
		this->clsData->surface->SetClipper(this->clsData->clipper);
	}
}

void Media::DDrawSurface::SetBuffSurface(Media::DDrawSurface *buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
