#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/DDrawSurface.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil.h"
#include "Media/ImageUtil_C.h"
#include <windows.h>
#include <ddraw.h>

struct Media::DDrawSurface::ClassData
{
	NN<Media::DDrawManager> mgr;
	LPDIRECTDRAW7 lpDD;
	LPDIRECTDRAWSURFACE7 surface;
	Optional<MonitorHandle> hMon;
	LPDIRECTDRAWCLIPPER clipper;
	Bool needRelease;
	Optional<Media::DDrawSurface> buffSurface;
};

Media::DDrawSurface::DDrawSurface(NN<DDrawManager> mgr, void *lpDD, void *surface, Optional<MonitorHandle> hMon, Bool needRelease, Media::RotateType rotateType)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->mgr = mgr;
	this->clsData->lpDD = (LPDIRECTDRAW7)lpDD;
	this->clsData->surface = (LPDIRECTDRAWSURFACE7)surface;
	this->clsData->hMon = hMon;
	this->clsData->clipper = 0;
	this->clsData->needRelease = needRelease;
	this->clsData->buffSurface = nullptr;

	DDSURFACEDESC2 ddsd;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	this->clsData->surface->GetSurfaceDesc(&ddsd);

	this->info.fourcc = 0;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.atype = Media::AT_IGNORE_ALPHA;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.storeBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
	this->info.pf = Media::PixelFormatGetDef(0, this->info.storeBPP);
	if (this->info.storeBPP == 32 && mgr->Is10BitColor(hMon))
	{
		this->info.pf = Media::PF_LE_A2B10G10R10;
	}
	this->info.dispSize.x = ddsd.dwWidth;
	this->info.dispSize.y = ddsd.dwHeight;
	this->info.storeSize.x = (UIntOS)ddsd.lPitch / (this->info.storeBPP >> 3);
	this->info.storeSize.y = ddsd.dwHeight;
	this->info.byteSize = this->info.storeSize.x * this->info.storeSize.y * (this->info.storeBPP >> 3);
	this->info.par2 = 1.0;
	this->info.hdpi = mgr->GetMonitorDPI(hMon);;
	this->info.vdpi = this->info.hdpi;
	NN<const Media::ColorProfile> color;
	if (mgr->GetMonProfile(hMon).SetTo(color))
		this->info.color.Set(color);
	else
		this->info.color.SetCommonProfile(Media::ColorProfile::CPT_VDISPLAY);
	this->info.rotateType = rotateType;
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

NN<Media::RasterImage> Media::DDrawSurface::Clone() const
{
	NN<DDrawSurface> ret;
	NEW_CLASSNN(ret, Media::DDrawSurface(this->clsData->mgr, this->clsData->lpDD, this->clsData->surface, this->clsData->hMon, false, this->info.rotateType));
	return ret;
}

Media::RasterImage::ImageType Media::DDrawSurface::GetImageType() const
{
	return Media::RasterImage::ImageType::MonitorSurface;
}

void Media::DDrawSurface::GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const
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
		if (left == 0 && top == 0 && width == ddsd.dwWidth && height == ddsd.dwHeight && ddsd.lPitch == (IntOS)destBpl && !upsideDown && ddsd.dwWidth * ((UIntOS)ddsd.ddpfPixelFormat.dwRGBBitCount >> 3) == destBpl && this->info.rotateType == destRotate)
		{
			MemCopyANC(destBuff.Ptr(), ddsd.lpSurface, destBpl * height);
		}
		else
		{
			Media::ImageUtil::ImageCopyR(destBuff, destBpl, (UInt8*)ddsd.lpSurface, ddsd.lPitch, left, top, width, height,
				ddsd.ddpfPixelFormat.dwRGBBitCount, upsideDown, this->info.rotateType, destRotate);
			//ImageCopy_ImgCopyR((UInt8*)ddsd.lpSurface + top * ddsd.lPitch + left * (Int32)(ddsd.ddpfPixelFormat.dwRGBBitCount >> 3), destBuff, width * (ddsd.ddpfPixelFormat.dwRGBBitCount >> 3), height, (UIntOS)(IntOS)ddsd.lPitch, destBpl, upsideDown);
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
	NN<Media::DDrawSurface> buffSurface;
	if (!this->clsData->buffSurface.SetTo(buffSurface))
	{
		return false;
	}
	HRESULT hRes = this->clsData->surface->Flip(0, 0);
	if (hRes == DDERR_SURFACELOST)
	{
		this->clsData->surface->Restore();
		((LPDIRECTDRAWSURFACE7)buffSurface->GetHandle())->Restore();
	}
	return hRes == DD_OK;
}

Bool Media::DDrawSurface::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank)
{
	IntOS drawWidth = (IntOS)this->info.dispSize.x;
	IntOS drawHeight = (IntOS)this->info.dispSize.y;
	Math::Size2D<UIntOS> dispSize = buffSize;
	Media::RotateType rt = Media::RotateTypeCalc(this->info.rotateType, surface->info.rotateType);
	RECT rc;
	HWND hWnd;
	if (this->clsData->clipper)
	{
		this->clsData->clipper->GetHWnd(&hWnd);
		GetClientRect(hWnd, &rc);
		ClientToScreen(hWnd, (POINT*)&rc.left);
		ClientToScreen(hWnd, (POINT*)&rc.right);
		drawWidth = ((IntOS)rc.right - rc.left);
		drawHeight = ((IntOS)rc.bottom - rc.top);

		MONITORINFOEXW info;
		info.cbSize = sizeof(info);
		if (GetMonitorInfoW((HMONITOR)this->clsData->hMon.OrNull(), &info))
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
	if (surface->info.storeBPP == this->info.storeBPP)
	{
		Bool succ = false;
		RECT rcSrc;
		DDSURFACEDESC2 ddsd;
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->info.dispSize.x;
		rcSrc.bottom = (LONG)this->info.dispSize.y;
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
			if (rt == Media::RotateType::CW_90 || rt == Media::RotateType::CW_270 || rt == Media::RotateType::HFLIP_CW_90 || rt == Media::RotateType::HFLIP_CW_270)
			{
				if (rc.right > (IntOS)this->info.dispSize.y)
				{
					rc.right = (LONG)(IntOS)this->info.dispSize.y;
				}
				if (rc.bottom > (IntOS)this->info.dispSize.x)
				{
					rc.bottom = (LONG)(IntOS)this->info.dispSize.x;
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
				drawWidth = (IntOS)rc.right - rc.left;
				drawHeight = (IntOS)rc.bottom - rc.top;
				if (destTL.x + (IntOS)buffSize.x > (IntOS)this->info.dispSize.y)
				{
					buffSize.x = (UIntOS)((IntOS)this->info.dispSize.y - destTL.x);
				}
				if (destTL.y + (IntOS)buffSize.y > (IntOS)this->info.dispSize.x)
				{
					buffSize.y = (UIntOS)((IntOS)this->info.dispSize.x - destTL.y);
				}
				dispSize = buffSize.SwapXY();
				destTL = destTL.SwapXY();
				IntOS tmp = drawX;
				drawX = drawY;
				drawY = tmp;
			}
			else
			{
				if (rc.right > (IntOS)this->info.dispSize.x)
				{
					rc.right = (LONG)(IntOS)this->info.dispSize.x;
				}
				if (rc.bottom > (IntOS)this->info.dispSize.y)
				{
					rc.bottom = (LONG)(IntOS)this->info.dispSize.y;
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
				drawWidth = (IntOS)rc.right - rc.left;
				drawHeight = (IntOS)rc.bottom - rc.top;
				if (destTL.x + (IntOS)buffSize.x > (IntOS)drawWidth)
				{
					buffSize.x = (UIntOS)(drawWidth - destTL.x);
				}
				if (destTL.y + (IntOS)buffSize.y > (IntOS)drawHeight)
				{
					buffSize.y = (UIntOS)(drawHeight - destTL.y);
				}
			}
			if ((IntOS)buffSize.x > 0 && (IntOS)buffSize.y > 0)
			{
				surface->GetRasterData((UInt8*)ddsd.lpSurface + (rc.top + destTL.y) * ddsd.lPitch + (rc.left + destTL.x) * ((IntOS)this->info.storeBPP >> 3), drawX, drawY, buffSize.x, buffSize.y, (UInt32)ddsd.lPitch, false, this->info.rotateType);
				if (clearScn)
				{
					if (destTL.y > 0)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (IntOS)rc.top * ddsd.lPitch + rc.left * ((IntOS)this->info.storeBPP >> 3), (UIntOS)drawWidth, (UIntOS)destTL.y, (UInt32)ddsd.lPitch, 0);
					}
					if (destTL.y + (IntOS)buffSize.y < (IntOS)drawHeight)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destTL.y + (IntOS)buffSize.y) * ddsd.lPitch + rc.left * ((IntOS)this->info.storeBPP >> 3), (UIntOS)drawWidth, (UIntOS)(drawHeight - (IntOS)buffSize.y - destTL.y), (UInt32)ddsd.lPitch, 0);
					}
					if (destTL.x > 0)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destTL.y) * ddsd.lPitch, (UIntOS)destTL.x, buffSize.y, (UInt32)ddsd.lPitch, 0);
					}
					if (destTL.x + (IntOS)buffSize.x < (IntOS)drawWidth)
					{
						ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (rc.top + destTL.y) * ddsd.lPitch + (destTL.x + (IntOS)buffSize.x) * (IntOS)(this->info.storeBPP >> 3), (UIntOS)drawWidth - (UIntOS)destTL.x - buffSize.x, buffSize.y, (UInt32)ddsd.lPitch, 0);
					}
				}
			}
			else if (clearScn)
			{
				ImageUtil_ImageColorFill32((UInt8*)ddsd.lpSurface + (IntOS)rc.top * ddsd.lPitch + rc.left * ((IntOS)this->info.storeBPP >> 3), (UIntOS)drawWidth, (UIntOS)drawHeight, (UInt32)ddsd.lPitch, 0);
			}
			this->clsData->surface->Unlock(0);
			succ = true;
		}
		else if (hRes == E_NOTIMPL)
		{
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
			if (rc.right > (IntOS)this->info.dispSize.x)
			{
				rc.right = (LONG)(IntOS)this->info.dispSize.x;
			}
			if (rc.bottom > (IntOS)this->info.dispSize.y)
			{
				rc.bottom = (LONG)(IntOS)this->info.dispSize.y;
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
			drawWidth = (IntOS)rc.right - rc.left;
			drawHeight = (IntOS)rc.bottom - rc.top;
			if (destTL.x + (IntOS)buffSize.x > (IntOS)drawWidth)
			{
				buffSize.x = (UIntOS)(drawWidth - destTL.x);
			}
			if (destTL.y + (IntOS)buffSize.y > (IntOS)drawHeight)
			{
				buffSize.y = (UIntOS)(drawHeight - destTL.y);
			}
			if ((IntOS)buffSize.x > 0 && (IntOS)buffSize.y > 0)
			{
				RECT rcSrc;
				rcSrc.left = (LONG)drawX;
				rcSrc.top = (LONG)drawY;
				rcSrc.right = rcSrc.left + (LONG)buffSize.x;
				rcSrc.bottom = rcSrc.top + (LONG)buffSize.y;
				RECT rcDest;
				rcDest.left = rc.left + (LONG)destTL.x;
				rcDest.top = rc.top + (LONG)destTL.y;
				rcDest.right = rcDest.left + (LONG)buffSize.x;
				rcDest.bottom = rcDest.top + (LONG)buffSize.y;
				hRes = this->clsData->surface->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)surface->GetHandle(), &rcSrc, DDBLT_WAIT, 0);
				if (hRes == DD_OK)
				{
					succ = true;
				}
				else
				{
					DDBLTFX bltfx;
					bltfx.dwFillColor = 0x00000000;
					this->clsData->surface->Blt(&rc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					clearScn = false;
				}
				if (clearScn)
				{
					DDBLTFX bltfx;
					bltfx.dwFillColor = 0x00000000;
					if (destTL.y > 0)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top;
						rcSrc.right = rcSrc.left + (LONG)drawWidth;
						rcSrc.bottom = rcSrc.top + (LONG)destTL.y;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destTL.y + (IntOS)buffSize.y < (IntOS)drawHeight)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top + (LONG)destTL.y + (LONG)buffSize.y;
						rcSrc.right = rcSrc.left + (LONG)drawWidth;
						rcSrc.bottom = rcSrc.top + (LONG)drawHeight - (LONG)destTL.y - (LONG)buffSize.y;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destTL.x > 0)
					{
						rcSrc.left = rc.left;
						rcSrc.top = rc.top + (LONG)destTL.y;
						rcSrc.right = rcSrc.left + (LONG)destTL.x;
						rcSrc.bottom = rcSrc.top + (LONG)buffSize.y;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
					if (destTL.x + (IntOS)buffSize.x < (IntOS)drawWidth)
					{
						rcSrc.left = rc.left + (LONG)destTL.x + (LONG)buffSize.x;
						rcSrc.top = rc.top + (LONG)destTL.y;
						rcSrc.right = rcSrc.left + (LONG)drawWidth - (LONG)destTL.x - (LONG)buffSize.x;
						rcSrc.bottom = rcSrc.top + (LONG)buffSize.y;
						this->clsData->surface->Blt(&rcSrc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
					}
				}
			}
			else if (clearScn)
			{
				DDBLTFX bltfx;
				bltfx.dwFillColor = 0x00000000;
				this->clsData->surface->Blt(&rc, 0, 0, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
				succ = true;
			}
		}
		return succ;
	}
	return false;
}

UnsafeArrayOpt<UInt8> Media::DDrawSurface::LockSurface(OutParam<IntOS> lineAdd)
{
	RECT rcSrc;
	HRESULT hRes;
	DDSURFACEDESC2 ddsd;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = (LONG)this->info.dispSize.x;
	rcSrc.bottom = (LONG)this->info.dispSize.y;

	hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
	if (hRes == DDERR_SURFACELOST)
	{
		this->clsData->surface->Release();
		hRes = this->clsData->surface->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
	}
	if (hRes == DD_OK)
	{
		lineAdd.Set(ddsd.lPitch);
		return (UInt8*)ddsd.lpSurface;
	}
	return nullptr;
}

void Media::DDrawSurface::UnlockSurface()
{
	this->clsData->surface->Unlock(0);
}

void Media::DDrawSurface::SetSurfaceBugMode(Bool surfaceBugMode)
{

}

void Media::DDrawSurface::SetClipWindow(Optional<ControlHandle> clipWindow)
{
	if (this->clsData->clipper)
	{
		this->clsData->clipper->Release();
		this->clsData->clipper = 0;
	}

	this->clsData->lpDD->CreateClipper(0, &this->clsData->clipper, NULL);
	if (this->clsData->clipper->SetHWnd(0, (HWND)clipWindow.OrNull()) != DD_OK)
	{
		this->clsData->clipper->Release();
		this->clsData->clipper = 0;
	}
	else
	{
		this->clsData->surface->SetClipper(this->clsData->clipper);
	}
}

void Media::DDrawSurface::SetBuffSurface(NN<Media::DDrawSurface> buffSurface)
{
	this->clsData->buffSurface = buffSurface;
}
