#include "Stdafx.h"
#include "Media/DRMMonitorSurfaceMgr.h"
#include "Media/DRMSurface.h"
#include "Media/MemorySurface.h"
#include "UI/GUICore.h"
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

Bool Media::DRMMonitorSurfaceMgr::OpenCard(UIntOS cardNum)
{
	Char sbuff[64];
	Text::StrUIntOS(Text::StrConcat(sbuff, "/dev/dri/card"), cardNum);
	this->clsData = MemAllocNN(ClassData);
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

Media::DRMMonitorSurfaceMgr::DRMMonitorSurfaceMgr(UIntOS cardNum, NN<UI::GUICore> ui, NN<Media::ColorManagerSess> colorSess)
{
	this->monMgr = nullptr;
	this->colorMgr = nullptr;
	this->colorSess = colorSess;
	this->ui = ui;
	this->OpenCard(cardNum);
}

Media::DRMMonitorSurfaceMgr::DRMMonitorSurfaceMgr(UIntOS cardNum, Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	this->colorSess = nullptr;
	this->ui = nullptr;
	this->OpenCard(cardNum);
}

Media::DRMMonitorSurfaceMgr::~DRMMonitorSurfaceMgr()
{
	if (this->clsData->fd >= 0)
	{
		drmDropMaster(this->clsData->fd);
		close(this->clsData->fd);
	}
	MemFreeNN(this->clsData);
}

Double Media::DRMMonitorSurfaceMgr::GetMonitorDPI(Optional<MonitorHandle> hMonitor)
{
	if (hMonitor.IsNull())
	{
		return 96.0;
	}

	NN<UI::GUICore> ui;
	NN<Media::MonitorMgr> monMgr;
	if (this->ui.SetTo(ui))
	{
		Double hdpi;
		Double vdpi;
		ui->GetMonitorDPIs(hMonitor, hdpi, vdpi);
		return hdpi;
	}
	else if (this->monMgr.SetTo(monMgr))
	{
		return monMgr->GetMonitorHDPI(hMonitor);
	}
	return 96.0;
}

Optional<const Media::ColorProfile> Media::DRMMonitorSurfaceMgr::GetMonitorColor(Optional<MonitorHandle> hMonitor)
{
	NN<Media::ColorManager> colorMgr;
	NN<Media::ColorManagerSess> colorSess;
	if (this->colorMgr.SetTo(colorMgr))
	{
		NN<Media::MonitorColorManager> monColor = colorMgr->GetMonColorManager(hMonitor);
		return monColor->GetRGBParam()->monProfile;
	}
	else if (this->colorSess.SetTo(colorSess))
	{
		return colorSess->GetRGBParam()->monProfile;
	}
	return nullptr;
}

Bool Media::DRMMonitorSurfaceMgr::Is10BitColor(Optional<MonitorHandle> hMonitor)
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

Bool Media::DRMMonitorSurfaceMgr::SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs)
{
	return true;
}

void Media::DRMMonitorSurfaceMgr::WaitForVBlank(Optional<MonitorHandle> hMon)
{
}

UInt32 Media::DRMMonitorSurfaceMgr::GetRefreshRate(Optional<MonitorHandle> hMon)
{
	IntOS index = -1 + (IntOS)hMon.OrNull();
	drmModeResPtr resources = drmModeGetResources(this->clsData->fd);
	if (resources == 0)
	{
		return 0;
	}
	UInt32 refreshRate = 0;
	drmModeConnectorPtr connector;
	IntOS cnt = 0;
	UIntOS i = 0;
	UIntOS j = (UInt32)resources->count_connectors;
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

Optional<MonitorHandle> Media::DRMMonitorSurfaceMgr::GetMonitorHandle(UIntOS monIndex)
{
	return (MonitorHandle*)(1 + monIndex);
}

UIntOS Media::DRMMonitorSurfaceMgr::GetMonitorCount()
{
	drmModeResPtr resources = drmModeGetResources(this->clsData->fd);
	if (resources == 0)
	{
		return 0;
	}
	drmModeConnectorPtr connector;
	UIntOS cnt = 0;
	UIntOS i = 0;
	UIntOS j = (UInt32)resources->count_connectors;
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

Optional<Media::MonitorSurface> Media::DRMMonitorSurfaceMgr::CreateSurface(Math::Size2D<UIntOS> size, UIntOS bitDepth)
{
	NN<Media::MemorySurface> surface;
	NEW_CLASSNN(surface, Media::MemorySurface(size, bitDepth, this->GetMonitorColor(nullptr), this->GetMonitorDPI(nullptr)));
	return surface;
}

Optional<Media::MonitorSurface> Media::DRMMonitorSurfaceMgr::CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType)
{
	NN<Media::DRMSurface> surface;
	NN<const Media::ColorProfile> color;
	if (!this->GetMonitorColor(hMon).SetTo(color))
	{
		return nullptr;
	}
	NEW_CLASSNN(surface, Media::DRMSurface(this->clsData->fd, hMon, color, this->GetMonitorDPI(hMon)));
	if (surface->IsError())
	{
		surface.Delete();
		return nullptr;
	}
	return surface;
}

Bool Media::DRMMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType)
{
	NN<Media::MonitorSurface> pSurface;
	if (this->CreatePrimarySurface(hMon, nullptr, Media::RotateType::None).SetTo(pSurface))
	{
		NN<Media::MonitorSurface> bSurface;
		if (this->CreateSurface(pSurface->info.dispSize, pSurface->info.storeBPP).SetTo(bSurface))
		{
			NN<Media::DRMSurface>::ConvertFrom(pSurface)->SetBuffSurface(bSurface);
			primarySurface.Set(pSurface);
			bufferSurface.Set(bSurface);
			return true;
		}
		pSurface.Delete();
	}
	return false;
}
