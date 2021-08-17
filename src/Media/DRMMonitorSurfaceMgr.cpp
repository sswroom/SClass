#include "Stdafx.h"
#include "Media/DRMMonitorSurfaceMgr.h"
#include "Media/DRMSurface.h"
#include "Media/MemorySurface.h"
#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <stdio.h>
#include <errno.h>

struct Media::DRMMonitorSurfaceMgr::ClassData
{
	Int32 fd;
	
};

Bool Media::DRMMonitorSurfaceMgr::OpenCard(UOSInt cardNum)
{
	Char sbuff[64];
	Text::StrUOSInt(Text::StrConcat(sbuff, "/dev/dri/card"), cardNum);
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->fd = open(sbuff, O_RDWR);
	if (this->clsData->fd < 0)
	{
		return false;
	}
	int ret = drmDropMaster(this->clsData->fd);
	if (ret < 0)
	{
		printf("drmDropMaster = %d\r\n", errno);
	}
	ret = drmSetMaster(this->clsData->fd);
	if (ret < 0)
	{
		printf("drmSetMaster = %d\r\n", errno);
	}
	return true;
}

Media::DRMMonitorSurfaceMgr::DRMMonitorSurfaceMgr(UOSInt cardNum, UI::GUICore *ui, Media::ColorManagerSess *colorSess)
{
	this->monMgr = 0;
	this->colorMgr = 0;
	this->colorSess = colorSess;
	this->ui = ui;
	this->OpenCard(cardNum);
}

Media::DRMMonitorSurfaceMgr::DRMMonitorSurfaceMgr(UOSInt cardNum, Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	this->colorSess = 0;
	this->ui = 0;
	this->OpenCard(cardNum);
}

Media::DRMMonitorSurfaceMgr::~DRMMonitorSurfaceMgr()
{
	if (this->clsData->fd >= 0)
	{
		drmDropMaster(this->clsData->fd);
		close(this->clsData->fd);
	}
	MemFree(this->clsData);
}

Double Media::DRMMonitorSurfaceMgr::GetMonitorDPI(MonitorHandle *hMonitor)
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

Media::ColorProfile *Media::DRMMonitorSurfaceMgr::GetMonitorColor(MonitorHandle *hMonitor)
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

Bool Media::DRMMonitorSurfaceMgr::Is10BitColor(MonitorHandle *hMonitor)
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

Bool Media::DRMMonitorSurfaceMgr::SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs)
{
	return true;
}

void Media::DRMMonitorSurfaceMgr::WaitForVBlank(MonitorHandle *hMon)
{
}

UInt32 Media::DRMMonitorSurfaceMgr::GetRefreshRate(MonitorHandle *hMon)
{
	OSInt index = -1 + (OSInt)hMon;
	drmModeResPtr resources = drmModeGetResources(this->clsData->fd);
	if (resources == 0)
	{
		return 0;
	}
	UInt32 refreshRate = 0;
	drmModeConnectorPtr connector;
	OSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = (UInt32)resources->count_connectors;
	while (i < j)
	{
		connector = drmModeGetConnector(this->clsData->fd, resources->connectors[i]);
		if (connector)
		{
			if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0)
			{
				if (index == cnt)
				{
					refreshRate = connector->modes[0].vrefresh;
				}
				cnt++;
			}
			drmModeFreeConnector(connector);
		}
		i++;
	}
	drmModeFreeResources(resources);
	return refreshRate;
}

MonitorHandle *Media::DRMMonitorSurfaceMgr::GetMonitorHandle(UOSInt monIndex)
{
	return (MonitorHandle*)(1 + monIndex);
}

UOSInt Media::DRMMonitorSurfaceMgr::GetMonitorCount()
{
	drmModeResPtr resources = drmModeGetResources(this->clsData->fd);
	if (resources == 0)
	{
		return 0;
	}
	drmModeConnectorPtr connector;
	UOSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = (UInt32)resources->count_connectors;
	while (i < j)
	{
		connector = drmModeGetConnector(this->clsData->fd, resources->connectors[i]);
		if (connector)
		{
			if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0)
			{
				cnt++;
			}
			drmModeFreeConnector(connector);
		}
		i++;
	}
	drmModeFreeResources(resources);
	return cnt;
}

Media::MonitorSurface *Media::DRMMonitorSurfaceMgr::CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth)
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(width, height, bitDepth, this->GetMonitorColor(0), this->GetMonitorDPI(0)));
	return surface;
}

Media::MonitorSurface *Media::DRMMonitorSurfaceMgr::CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow)
{
	Media::DRMSurface *surface = 0;
	NEW_CLASS(surface, Media::DRMSurface(this->clsData->fd, hMon, this->GetMonitorColor(hMon), this->GetMonitorDPI(hMon)));
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

Bool Media::DRMMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface)
{
	Media::MonitorSurface *pSurface = this->CreatePrimarySurface(hMon, 0);
	if (pSurface)
	{
		Media::MonitorSurface *bSurface = this->CreateSurface(pSurface->info->dispWidth, pSurface->info->dispHeight, pSurface->info->storeBPP);
		if (bSurface)
		{
			((Media::DRMSurface*)pSurface)->SetBuffSurface(bSurface);
			*primarySurface = pSurface;
			*bufferSurface = bSurface;
			return true;
		}
		DEL_CLASS(pSurface);
	}
	return false;
}
