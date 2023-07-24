#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
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

const Media::ColorProfile *Media::FBMonitorSurfaceMgr::GetMonitorColor(MonitorHandle *hMonitor)
{
	if (this->colorMgr)
	{
		Media::MonitorColorManager *monColor = this->colorMgr->GetMonColorManager(hMonitor);
		return &monColor->GetRGBParam()->monProfile;
	}
	else if (this->colorSess)
	{
		return &this->colorSess->GetRGBParam()->monProfile;
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

Media::MonitorSurface *Media::FBMonitorSurfaceMgr::CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth)
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(size, bitDepth, this->GetMonitorColor(0), this->GetMonitorDPI(0)));
	return surface;
}

Media::MonitorSurface *Media::FBMonitorSurfaceMgr::CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow, Media::RotateType rotateType)
{
	return 0;
}

Bool Media::FBMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface, Media::RotateType rotateType)
{
	return false;
}
