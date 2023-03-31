#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/DDrawSurface.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
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

Media::DDrawSurface::DDrawSurface(DDrawManager *mgr, void *lpDD, void *surface, MonitorHandle *hMon, Bool needRelease, Media::RotateType rotateType)
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

	this->info.fourcc = 0;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.atype = Media::AT_NO_ALPHA;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.storeBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
	this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
	if (this->info.storeBPP == 32 && mgr->Is10BitColor(hMon))
	{
		this->info.pf = Media::PF_LE_A2B10G10R10;
	}
	this->info.dispWidth = ddsd.dwWidth;
	this->info.dispHeight = ddsd.dwHeight;
	this->info.storeWidth = (UOSInt)ddsd.lPitch / (this->info.storeBPP >> 3);
	this->info.storeHeight = ddsd.dwHeight;
	this->info.byteSize = this->info.storeWidth * this->info.storeHeight * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = mgr->GetMonitorDPI(hMon);;
	this->info.vdpi = this->info.hdpi;
	this->info.color->Set(mgr->GetMonProfile(hMon));
	this->info.rotateType = Media::RotateType::None; //rotateType;
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

Media::Image *Media::DDrawSurface::Clone() const
{
	return 0;
}

Media::Image::ImageType Media::DDrawSurface::GetImageType() const
{
	return Media::Image::ImageType::MonitorSurface;
}

void Media::DDrawSurface::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
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
		if (left == 0 && top == 0 && width == ddsd.dwWidth && height == ddsd.dwHeight && ddsd.lPitch == (OSInt)destBpl && !upsideDown && ddsd.dwWidth * ((UOSInt)ddsd.ddpfPixelFormat.dwRGBBitCount >> 3) == destBpl && this->info.rotateType == destRotate)
		{
			MemCopyANC(destBuff, ddsd.lpSurface, destBpl * height);
		}
		else
		{
			ImageCopy_ImgCopyR((UInt8*)ddsd.lpSurface + top * ddsd.lPitch + left * (Int32)(ddsd.ddpfPixelFormat.dwRGBBitCount >> 3), destBuff, width * (ddsd.ddpfPixelFormat.dwRGBBitCount >> 3), height, (UOSInt)(OSInt)ddsd.lPitch, destBpl, upsideDown);
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

Bool Media::DDrawSurface::DrawFromSurface(Media::MonitorSurface *surface, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank)
{
	OSInt drawWidth = (OSInt)this->info.dispWidth;
	OSInt drawHeight = (OSInt)this->info.dispHeight;
	RECT rc;
	HWND hWnd;
	if (this->clsData->clipper)
	{
		this->clsData->clipper->GetHWnd(&hWnd);
		GetClientRect(hWnd, &rc);
		ClientToScreen(hWnd, (POINT*)&rc.left);
		ClientToScreen(hWnd, (POINT*)&rc.right);
		drawWidth = ((OSInt)rc.right - rc.left);
		drawHeight = ((OSInt)rc.bottom - rc.top);

		MONITORINFOEXW info;
		info.cbSize = sizeof(info);
		if (GetMonitorInfoW((HMONITOR)this->clsData->hMon, &info))
		{
			rc.left -= info.rcMonitor.left;
			rc.top -= info.rcMonitor.top;
			rc.right -= info.rcMonitor.left;
			rc.bottom -= info.rcMonitor.bottom;
		}
	}
	else
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = (LONG)drawWidth;
		rc.bottom = (LONG)drawHeight;
	}
	if (surface && surface->info.storeBPP == this->info.storeBPP)
	{
		Bool succ = false;
		RECT rcSrc;
		DDSURFACEDESC2 ddsd;
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->info.dispWidth;
		rcSrc.bottom = (LONG)this->info.dispHeight;
		MemClear(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		HRESULT hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
		if (hRes == DDERR_SURFACELOST)
		{
			this->clsData->surface->Release();
			hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
		}
		if (hRes == DD_OK)
		{
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
			if (rc.right > (OSInt)this->info.dispWidth)
			{
				rc.right = (LONG)(OSInt)this->info.dispWidth;
			}
			if (rc.bottom > (OSInt)this->info.dispHeight)
			{
				rc.bottom = (LONG)(OSInt)this->info.dispHeight;
			}
			if (rc.left < 0)
			{
				drawX += -rc.left;
				rc.left = 0;
			}
			if (rc.top < 0)
			{
				drawY += -rc.top;
				rc.top = 0;
			}
			drawWidth = (OSInt)rc.right - rc.left;
			drawHeight = (OSInt)rc.bottom - rc.top;
			if (destX + (OSInt)buffW > (OSInt)drawWidth)
			{
				buffW = (UOSInt)(drawWidth - destX);
			}
			if (destY + (OSInt)buffH > (OSInt)drawHeight)
			{
				buffH = (UOSInt)(drawHeight - destY);
			}
			if ((OSInt)buffW > 0 && (OSInt)buffH > 0)
			{
				surface->GetImageData((UInt8*)ddsd.lpSurface + (rc.top + destY) * ddsd.lPitch + (rc.left + destX) * ((OSInt)this->info.storeBPP >> 3), drawX, drawY, buffW, buffH, (UInt32)ddsd.lPitch, false, this->info.rotateType);
				if (clearScn)
				{
					if (destY > 0)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (OSInt)rc.top * ddsd.lPitch + rc.left * ((OSInt)this->info.storeBPP >> 3), (UOSInt)drawWidth, (UOSInt)destY, (UInt32)ddsd.lPitch, 0);
					}
					if (destY + (OSInt)buffH < (OSInt)drawHeight)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destY + (OSInt)buffH) * ddsd.lPitch + rc.left * ((OSInt)this->info.storeBPP >> 3), (UOSInt)drawWidth, (UOSInt)(drawHeight - (OSInt)buffH - destY), (UInt32)ddsd.lPitch, 0);
					}
					if (destX > 0)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destY) * ddsd.lPitch, (UOSInt)destX, buffH, (UInt32)ddsd.lPitch, 0);
					}
					if (destX + (OSInt)buffW < (OSInt)drawWidth)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destY) * ddsd.lPitch + (destX + (OSInt)buffW) * (OSInt)(this->info.storeBPP >> 3), (UOSInt)drawWidth - (UOSInt)destX - buffW, buffH, (UInt32)ddsd.lPitch, 0);
					}
				}
			}
			else if (clearScn)
			{
				ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (OSInt)rc.top * ddsd.lPitch + rc.left * ((OSInt)this->info.storeBPP >> 3), (UOSInt)drawWidth, (UOSInt)drawHeight, (UInt32)ddsd.lPitch, 0);
			}
			this->clsData->surface->Unlock(0);
			succ = true;
		}
		else if (hRes == E_NOTIMPL)
		{
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
			if (rc.right > (OSInt)this->info.dispWidth)
			{
				rc.right = (LONG)(OSInt)this->info.dispWidth;
			}
			if (rc.bottom > (OSInt)this->info.dispHeight)
			{
				rc.bottom = (LONG)(OSInt)this->info.dispHeight;
			}
			if (rc.left < 0)
			{
				drawX += -rc.left;
				rc.left = 0;
			}
			if (rc.top < 0)
			{
				drawY += -rc.top;
				rc.top = 0;
			}
			drawWidth = (OSInt)rc.right - rc.left;
			drawHeight = (OSInt)rc.bottom - rc.top;
			if (destX + (OSInt)buffW > (OSInt)drawWidth)
			{
				buffW = (UOSInt)(drawWidth - destX);
			}
			if (destY + (OSInt)buffH > (OSInt)drawHeight)
			{
				buffH = (UOSInt)(drawHeight - destY);
			}
			if ((OSInt)buffW > 0 && (OSInt)buffH > 0)
			{
				RECT rcSrc;
				rcSrc.left = (LONG)drawX;
				rcSrc.top = (LONG)drawY;
				rcSrc.right = rcSrc.left + (LONG)buffW;
				rcSrc.bottom = rcSrc.top + (LONG)buffH;
				RECT rcDest;
				rcDest.left = rc.left + (LONG)destX;
				rcDest.top = rc.top + (LONG)destY;
				rcDest.right = rcDest.left + (LONG)buffW;
				rcDest.bottom = rcDest.top + (LONG)buffH;
				this->clsData->surface->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)surface->GetHandle(), &rcSrc, DDBLT_WAIT, 0);
				if (clearScn)
				{
					DDBLTFX bltfx;
					bltfx.dwFillColor = 0x00000000;
					if (destY > 0)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top;
						rcSrc.right = rcSrc.left + (LONG)drawWidth;
						rcSrc.bottom = rcSrc.top + (LONG)destY;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destY + (OSInt)buffH < (OSInt)drawHeight)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top + (LONG)destY + (LONG)buffH;
						rcSrc.right = rcSrc.left + (LONG)drawWidth;
						rcSrc.bottom = rcSrc.top + (LONG)drawHeight - (LONG)destY - (LONG)buffH;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destX > 0)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top + (LONG)destY;
						rcSrc.right = rcSrc.left + (LONG)destX;
						rcSrc.bottom = rcSrc.top + (LONG)buffH;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destX + (OSInt)buffW < (OSInt)drawWidth)
					{
						rcSrc.left = rc.left + (LONG)destX + (LONG)buffW;
						rcSrc.top = rc.top + (LONG)destY;
						rcSrc.right = rcSrc.left + (LONG)drawWidth - (LONG)destX - (LONG)buffW;
						rcSrc.bottom = rcSrc.top + (LONG)buffH;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
				}
			}
			else if (clearScn)
			{
				DDBLTFX bltfx;
				bltfx.dwFillColor = 0x00000000;
				this->clsData->surface->Blt(&rc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
			}
		}
		return succ;
	}
	return false;
}

UInt8 *Media::DDrawSurface::LockSurface(OSInt *lineAdd)
{
	RECT rcSrc;
	HRESULT hRes;
	DDSURFACEDESC2 ddsd;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = (LONG)this->info.dispWidth;
	rcSrc.bottom = (LONG)this->info.dispHeight;

	hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
	if (hRes == DDERR_SURFACELOST)
	{
		this->clsData->surface->Release();
		hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
	}
	if (hRes == DD_OK)
	{
		*lineAdd = ddsd.lPitch;
		return (UInt8*)ddsd.lpSurface;
	}
	return 0;
}

void Media::DDrawSurface::UnlockSurface()
{
	this->clsData->surface->Unlock(0);
}

void Media::DDrawSurface::SetSurfaceBugMode(Bool surfaceBugMode)
{

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
