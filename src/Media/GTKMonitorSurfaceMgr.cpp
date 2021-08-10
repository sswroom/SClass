#include "Stdafx.h"
#include "Media/FBSurface.h"
#include "Media/GTKMonitorSurfaceMgr.h"

Media::GTKMonitorSurfaceMgr::GTKMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
}

Media::GTKMonitorSurfaceMgr::~GTKMonitorSurfaceMgr()
{

}

Bool Media::GTKMonitorSurfaceMgr::SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs)
{
	return true;
}

void Media::GTKMonitorSurfaceMgr::WaitForVBlank(MonitorHandle *hMon)
{
}

UInt32 Media::GTKMonitorSurfaceMgr::GetRefreshRate(MonitorHandle *hMon)
{
	return 0;
}

Media::MonitorSurface *Media::GTKMonitorSurfaceMgr::CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth)
{
	return 0;
}

Media::MonitorSurface *Media::GTKMonitorSurfaceMgr::CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow)
{
	Media::MonitorColorManager *monColor = this->colorMgr->GetMonColorManager(hMon);
	Media::FBSurface *surface;
	NEW_CLASS(surface, Media::FBSurface(hMon, monColor->GetRGBParam()->monProfile, this->monMgr->GetMonitorHDPI(hMon)));
	if (surface->IsError())
	{
		DEL_CLASS(surface);
		return 0;
	}
	return surface;
}

Bool Media::GTKMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface)
{
	return 0;
}
