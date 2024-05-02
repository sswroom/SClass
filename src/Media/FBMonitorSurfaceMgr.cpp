#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
#include "Media/FBSurface.h"
#include "Media/MemorySurface.h"

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess)
{
	this->monMgr = 0;
	this->colorMgr = 0;
	this->colorSess = colorSess;
}

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	this->colorSess = 0;
}

Media::FBMonitorSurfaceMgr::~FBMonitorSurfaceMgr()
{

}

Double Media::FBMonitorSurfaceMgr::GetMonitorDPI(MonitorHandle *hMonitor)
{
	if (hMonitor == 0)
	{
		return 96.0;
	}

	NN<Media::MonitorMgr> monMgr;
	if (this->monMgr.SetTo(monMgr))
	{
		return monMgr->GetMonitorHDPI(hMonitor);
	}
	return 96.0;
}

const Media::ColorProfile *Media::FBMonitorSurfaceMgr::GetMonitorColor(MonitorHandle *hMonitor)
{
	NN<Media::ColorManager> colorMgr;
	NN<Media::ColorManagerSess> colorSess;
	if (this->colorMgr.SetTo(colorMgr))
	{
		NN<Media::MonitorColorManager> monColor = colorMgr->GetMonColorManager(hMonitor);
		return &monColor->GetRGBParam()->monProfile;
	}
	else if (this->colorSess.SetTo(colorSess))
	{
		return &colorSess->GetRGBParam()->monProfile;
	}
	return 0;
}

Bool Media::FBMonitorSurfaceMgr::Is10BitColor(MonitorHandle *hMonitor)
{
	NN<Media::ColorManager> colorMgr;
	NN<Media::ColorManagerSess> colorSess;
	if (this->colorMgr.SetTo(colorMgr))
	{
		return colorMgr->GetMonColorManager(hMonitor)->Get10BitColor();
	}
	else if (this->colorSess.SetTo(colorSess))
	{
		return colorSess->Get10BitColor();
	}
	return false;
}

Bool Media::FBMonitorSurfaceMgr::SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs)
{
	return true;
}

void Media::FBMonitorSurfaceMgr::WaitForVBlank(MonitorHandle *hMon)
{
}

UInt32 Media::FBMonitorSurfaceMgr::GetRefreshRate(MonitorHandle *hMon)
{
	return 0;
}

MonitorHandle *Media::FBMonitorSurfaceMgr::GetMonitorHandle(UOSInt monIndex)
{
	return (MonitorHandle*)(1 + monIndex);
}

UOSInt Media::FBMonitorSurfaceMgr::GetMonitorCount()
{
	return 1;
}

Media::MonitorSurface *Media::FBMonitorSurfaceMgr::CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth)
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(size, bitDepth, this->GetMonitorColor(0), this->GetMonitorDPI(0)));
	return surface;
}

Media::MonitorSurface *Media::FBMonitorSurfaceMgr::CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow, Media::RotateType rotateType)
{
	Media::FBSurface *surface = 0;
	NEW_CLASS(surface, Media::FBSurface(hMon, this->GetMonitorColor(hMon), this->GetMonitorDPI(hMon), rotateType));
	if (surface == 0)
	{
		return 0;
	}
	else if (surface->IsError())
	{
		DEL_CLASS(surface);
		return 0;
	}
	return surface;
}

Bool Media::FBMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface, Media::RotateType rotateType)
{
	Media::MonitorSurface *pSurface = this->CreatePrimarySurface(hMon, 0, rotateType);
	if (pSurface)
	{
		Media::MonitorSurface *bSurface = this->CreateSurface(pSurface->info.dispSize, pSurface->info.storeBPP);
		if (bSurface)
		{
			((Media::FBSurface*)pSurface)->SetBuffSurface(bSurface);
			*primarySurface = pSurface;
			*bufferSurface = bSurface;
			return true;
		}
		DEL_CLASS(pSurface);
	}
	return false;
}
