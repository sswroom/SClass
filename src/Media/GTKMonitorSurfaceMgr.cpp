#include "Stdafx.h"
#include "Media/FBSurface.h"
#include "Media/GTKMonitorSurfaceMgr.h"
#include "Media/MemorySurface.h"

Media::GTKMonitorSurfaceMgr::GTKMonitorSurfaceMgr(UI::GUICore *ui, Media::ColorManagerSess *colorSess)
{
	this->monMgr = 0;
	this->colorMgr = 0;
	this->colorSess = colorSess;
	this->ui = ui;
}

Media::GTKMonitorSurfaceMgr::GTKMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	this->colorSess = 0;
	this->ui = 0;
}

Media::GTKMonitorSurfaceMgr::~GTKMonitorSurfaceMgr()
{

}

Double Media::GTKMonitorSurfaceMgr::GetMonitorDPI(MonitorHandle *hMonitor)
{
	if (hMonitor == 0)
	{
		return 96.0;
	}

	if (this->ui)
	{
		Double hdpi;
		Double vdpi;
		this->ui->GetMonitorDPIs(hMonitor, &hdpi, &vdpi);
		return hdpi;
	}
	else if (this->monMgr)
	{
		return this->monMgr->GetMonitorHDPI(hMonitor);
	}
	return 96.0;
}

Media::ColorProfile *Media::GTKMonitorSurfaceMgr::GetMonitorColor(MonitorHandle *hMonitor)
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

Bool Media::GTKMonitorSurfaceMgr::Is10BitColor(MonitorHandle *hMonitor)
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
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(width, height, bitDepth, this->GetMonitorColor(0), this->GetMonitorDPI(0)));
	return surface;
}

Media::MonitorSurface *Media::GTKMonitorSurfaceMgr::CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow)
{
	Media::FBSurface *surface = 0;
	NEW_CLASS(surface, Media::FBSurface(hMon, this->GetMonitorColor(hMon), this->GetMonitorDPI(hMon)));
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

Bool Media::GTKMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface)
{
	Media::MonitorSurface *pSurface = this->CreatePrimarySurface(hMon, 0);
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
