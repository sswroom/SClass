#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DDrawManager.h"
#include "Media/ScreenCapturer.h"
#include <windows.h>
#include <ddraw.h>

Media::ScreenCapturer::ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	NEW_CLASS(this->clsData, Media::DDrawManager());
}

Media::ScreenCapturer::~ScreenCapturer()
{
	Media::DDrawManager *ddMgr = (Media::DDrawManager*)this->clsData;
	DEL_CLASS(ddMgr);
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(void *hMon)
{
	Media::DDrawManager *ddMgr = (Media::DDrawManager*)this->clsData;
	if (ddMgr->IsError())
	{
		return 0;
	}
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddMgr->GetDD7(hMon);
	if (lpDD == 0)
	{
		return 0;
	}
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	primarySurface = 0;
	HRESULT res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
	if (res == DDERR_NOCOOPERATIVELEVELSET)
	{
		lpDD->SetCooperativeLevel((HWND)0, DDSCL_NORMAL);
		res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
	}
	if (res != DD_OK)
	{
		return 0;
	}

	Media::StaticImage *retImg = 0;
	MemClear(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	res = primarySurface->Lock(0, &ddsd, DDLOCK_WAIT, 0);
	if (res == DDERR_SURFACELOST)
	{
		primarySurface->Restore();
		res = primarySurface->Lock(0, &ddsd, DDLOCK_WAIT, 0);
	}
	if (res == DD_OK)
	{
		Media::MonitorColorManager *monColor = this->colorMgr->GetMonColorManager(hMon);
		Bool valid = true;
		Media::FrameInfo info;
		info.fourcc = 0;
		info.ftype = Media::FT_NON_INTERLACE;
		info.atype = Media::AT_NO_ALPHA;
		info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
		info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		info.storeBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
		info.pf = Media::FrameInfo::GetDefPixelFormat(0, info.storeBPP);
		if (info.storeBPP == 32 && monColor->Get10BitColor())
		{
			info.pf = Media::PF_LE_A2B10G10R10;
		}
		info.dispWidth = ddsd.dwWidth;
		info.dispHeight = ddsd.dwHeight;
		info.storeWidth = (UOSInt)ddsd.lPitch / (info.storeBPP >> 3);
		info.storeHeight = ddsd.dwHeight;
		info.byteSize = info.storeWidth * info.storeHeight * (info.storeBPP >> 3);
		info.par2 = 1.0;
		info.hdpi = this->monMgr->GetMonitorHDPI(hMon);;
		info.vdpi = info.hdpi;
		const Media::IColorHandler::RGBPARAM2 *params = monColor->GetRGBParam();
		info.color->Set(params->monProfile);
		
		if (valid)
		{
			NEW_CLASS(retImg, Media::StaticImage(&info));
			MemCopyNANC(retImg->data, ddsd.lpSurface, info.byteSize);
		}
		primarySurface->Unlock(0);
	}
	primarySurface->Release();

	return retImg;
}
