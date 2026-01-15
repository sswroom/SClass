#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
#include "Media/FBSurface.h"
#include "Media/MemorySurface.h"

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess)
{
	this->monMgr = nullptr;
	this->colorMgr = nullptr;
	this->colorSess = colorSess;
}

Media::FBMonitorSurfaceMgr::FBMonitorSurfaceMgr(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	this->colorSess = nullptr;
}

Media::FBMonitorSurfaceMgr::~FBMonitorSurfaceMgr()
{

}

Double Media::FBMonitorSurfaceMgr::GetMonitorDPI(Optional<MonitorHandle> hMonitor)
{
	if (hMonitor.IsNull())
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

Optional<const Media::ColorProfile> Media::FBMonitorSurfaceMgr::GetMonitorColor(Optional<MonitorHandle> hMonitor)
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
	return nullptr;
}

Bool Media::FBMonitorSurfaceMgr::Is10BitColor(Optional<MonitorHandle> hMonitor)
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

Bool Media::FBMonitorSurfaceMgr::SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs)
{
	return true;
}

void Media::FBMonitorSurfaceMgr::WaitForVBlank(Optional<MonitorHandle> hMon)
{
}

UInt32 Media::FBMonitorSurfaceMgr::GetRefreshRate(Optional<MonitorHandle> hMon)
{
	return 0;
}

Optional<MonitorHandle> Media::FBMonitorSurfaceMgr::GetMonitorHandle(UIntOS monIndex)
{
	return (MonitorHandle*)(1 + monIndex);
}

UIntOS Media::FBMonitorSurfaceMgr::GetMonitorCount()
{
	return 1;
}

Optional<Media::MonitorSurface> Media::FBMonitorSurfaceMgr::CreateSurface(Math::Size2D<UIntOS> size, UIntOS bitDepth)
{
	Media::MemorySurface *surface;
	NEW_CLASS(surface, Media::MemorySurface(size, bitDepth, this->GetMonitorColor(nullptr), this->GetMonitorDPI(nullptr)));
	return surface;
}

Optional<Media::MonitorSurface> Media::FBMonitorSurfaceMgr::CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType)
{
	Media::FBSurface *surface = nullptr;
	NEW_CLASS(surface, Media::FBSurface(hMon, this->GetMonitorColor(hMon), this->GetMonitorDPI(hMon), rotateType));
	if (surface == nullptr)
	{
		return nullptr;
	}
	else if (surface->IsError())
	{
		DEL_CLASS(surface);
		return nullptr;
	}
	return surface;
}

Bool Media::FBMonitorSurfaceMgr::CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType)
{
	NN<Media::MonitorSurface> pSurface;
	if (this->CreatePrimarySurface(hMon, nullptr, rotateType).SetTo(pSurface))
	{
		NN<Media::MonitorSurface> bSurface;
		if (this->CreateSurface(pSurface->info.dispSize, pSurface->info.storeBPP).SetTo(bSurface))
		{
			NN<Media::FBSurface>::ConvertFrom(pSurface)->SetBuffSurface(bSurface);
			primarySurface.Set(pSurface);
			bufferSurface.Set(bSurface);
			return true;
		}
		pSurface.Delete();
	}
	return false;
}
