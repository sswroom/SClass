#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
#include "Media/FBSurface.h"
#include "Media/MemorySurface.h"

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	this->monMgr = 0;
	this->colorMgr = 0;
	this->colorSess = colorSess;
}

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
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

	if (this->monMgr)
	{
		return this->monMgr->GetMonitorHDPI(hMonitor);
	}
	return 96.0;
}

Media::ColorProfile *Media::FBMonitorSurfaceMgr::GetMonitorColor(MonitorHandle *hMonitor)
{
	if (this->colorMgr)
	{
		Media::MonitorColorManager *monColor = this->colorMgr->GetMonColorManager(hMonitor);
		return monColor->GetRGBParam()->monProfile;
	}
	else if (this->colorSess)
	{
		return this->colorSess->GetRGBParam()->monProfile;
	}
	return 0;
}

Bool Media::FBMonitorSurfaceMgr::Is10BitColor(MonitorHandle *hMonitor)
{
	if (this->colorMgr)
	{
		return this->colorMgr->GetMonColorManager(hMonitor)->Get10BitColor();
	}
	else if (this->colorSess)
	{
		return this->colorSess->Get10BitColor();
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

Media::MonitorSurface *Media::FBMonitorSurfaceMgr::CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth)
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(width, height, bitDepth, this->GetMonitorColor(0), this->GetMonitorDPI(0)));
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
		Media::MonitorSurface *bSurface = this->CreateSurface(pSurface->info->dispWidth, pSurface->info->dispHeight, pSurface->info->storeBPP);
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
