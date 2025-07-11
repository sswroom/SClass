#include "Stdafx.h"
#include "Data/FastMap.h"
#include "Media/DDrawManager.h"
#include "Media/DDrawSurface.h"
#include <windows.h>
#include <ddraw.h>

struct Media::DDrawManager::ClassData
{
	Data::FastMap<OSInt, LPDIRECTDRAW7> monMap;
	LPDIRECTDRAW7 defDD;
	Optional<Media::MonitorMgr> monMgr;
	Optional<Media::ColorManager> colorMgr;
	Optional<Media::ColorManagerSess> colorSess;
};

Int32 __stdcall Media::DDrawManager::DDEnumMonCall(void *guid, Char *driverDesc, Char *driverName, void *context, void *hMonitor)
{
	Media::DDrawManager *me = (Media::DDrawManager*)context;
	LPDIRECTDRAW7 lpDD;
	if (hMonitor == 0 && me->clsData->defDD != 0)
	{
		return 1;
	}
	if (me->clsData->monMap.Get((OSInt)hMonitor) != 0)
	{
		return 1;
	}

	if (DirectDrawCreateEx( (GUID FAR *)guid, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		if (hMonitor == 0)
		{
			me->clsData->defDD = lpDD;
		}
		else
		{
			me->clsData->monMap.Put((OSInt)hMonitor, lpDD);
		}
	}

	return 1;
}

void Media::DDrawManager::ReleaseAll()
{
	if (this->clsData->defDD)
	{
		this->clsData->defDD->Release();
		this->clsData->defDD = 0;
	}
	UOSInt i;
	i = this->clsData->monMap.GetCount();
	while (i-- > 0)
	{
		this->clsData->monMap.GetItem(i)->Release();
	}
	this->clsData->monMap.Clear();
}

Media::DDrawManager::DDrawManager(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess)
{
	NEW_CLASS(this->clsData, ClassData());
	this->clsData->defDD = 0;
	this->clsData->monMgr = 0;
	this->clsData->colorMgr = 0;
	this->clsData->colorSess = colorSess;
	this->RecheckMonitor();
}

Media::DDrawManager::DDrawManager(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	NEW_CLASS(this->clsData, ClassData());
	this->clsData->defDD = 0;
	this->clsData->monMgr = monMgr;
	this->clsData->colorMgr = colorMgr;
	this->clsData->colorSess = 0;
	this->RecheckMonitor();
}

Media::DDrawManager::~DDrawManager()
{
	this->ReleaseAll();
	DEL_CLASS(this->clsData);
}

Bool Media::DDrawManager::IsError()
{
	return this->clsData->defDD == 0;
}

void *Media::DDrawManager::GetDD7(Optional<MonitorHandle> hMonitor)
{
	this->RecheckMonitor();
	LPDIRECTDRAW7 ret = this->clsData->monMap.Get((OSInt)hMonitor.OrNull());
	if (ret)
		return ret;
	return this->clsData->defDD;
}

void Media::DDrawManager::ReleaseDD7(Optional<MonitorHandle> hMonitor)
{
	LPDIRECTDRAW7 ret = this->clsData->monMap.Remove((OSInt)hMonitor.OrNull());
	if (ret)
	{
		ret->Release();
	}
}

void Media::DDrawManager::RecheckMonitor()
{
	DirectDrawEnumerateExA((LPDDENUMCALLBACKEXA)DDEnumMonCall, this, DDENUM_ATTACHEDSECONDARYDEVICES);
}

void Media::DDrawManager::Reinit()
{
	this->ReleaseAll();
	this->RecheckMonitor();
}

Double Media::DDrawManager::GetMonitorDPI(Optional<MonitorHandle> hMonitor)
{
	if (hMonitor.IsNull())
	{
		return 96.0;
	}

	NN<Media::MonitorMgr> monMgr;
	if (this->clsData->monMgr.SetTo(monMgr))
	{
		return monMgr->GetMonitorHDPI(hMonitor);
	}
	return 96.0;
}

Bool Media::DDrawManager::Is10BitColor(Optional<MonitorHandle> hMonitor)
{
	NN<Media::ColorManager> colorMgr;
	NN<Media::ColorManagerSess> colorSess;
	if (this->clsData->colorMgr.SetTo(colorMgr))
	{
		return colorMgr->GetMonColorManager(hMonitor)->Get10BitColor();
	}
	else if (this->clsData->colorSess.SetTo(colorSess))
	{
		return colorSess->Get10BitColor();
	}
	return false;
}

Optional<const Media::ColorProfile> Media::DDrawManager::GetMonProfile(Optional<MonitorHandle> hMonitor)
{
	NN<Media::ColorManager> colorMgr;
	NN<Media::ColorManagerSess> colorSess;
	if (this->clsData->colorMgr.SetTo(colorMgr))
	{
		return &colorMgr->GetMonColorManager(hMonitor)->GetRGBParam()->monProfile;
	}
	else if (this->clsData->colorSess.SetTo(colorSess))
	{
		return &colorSess->GetRGBParam()->monProfile;
	}
	return 0;
}

Bool Media::DDrawManager::SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs)
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(hMon);
	if (lpDD == 0) return false;
	if (fs)
	{
		return lpDD->SetCooperativeLevel((HWND)hWnd.OrNull(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) == DD_OK;
	}
	else
	{
		return lpDD->SetCooperativeLevel((HWND)hWnd.OrNull(), DDSCL_NORMAL) == DD_OK;
	}
}

void Media::DDrawManager::WaitForVBlank(Optional<MonitorHandle> hMon)
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(hMon);
	if (lpDD)
	{
		lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
	}
}

UInt32 Media::DDrawManager::GetRefreshRate(Optional<MonitorHandle> hMon)
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(hMon);
	if (lpDD == 0)
		return 0;
	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(ddsd);
	if (lpDD->GetDisplayMode(&ddsd) == DD_OK)
	{
		return ddsd.dwRefreshRate;
	}
	else
	{
		return 0;
	}
}

BOOL CALLBACK DDrawManager_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	UOSInt *data = (UOSInt*)dwData;
	if (data[0] == data[1])
	{
		data[2] = (UOSInt)hMonitor;
	}
	data[0]++;
    return TRUE;
}

Optional<MonitorHandle> Media::DDrawManager::GetMonitorHandle(UOSInt monIndex)
{
	UOSInt arrs[3];
	arrs[0] = 0;
	arrs[1] = monIndex;
	arrs[2] = 0;
	if (EnumDisplayMonitors(NULL, NULL, DDrawManager_MonitorEnumProc, (LPARAM)&arrs))
		return (MonitorHandle*)arrs[2];
	return 0;
}

UOSInt Media::DDrawManager::GetMonitorCount()
{
	UOSInt arrs[3];
	arrs[0] = 0;
	arrs[1] = 0;
	arrs[2] = 0;
	if (EnumDisplayMonitors(NULL, NULL, DDrawManager_MonitorEnumProc, (LPARAM)&arrs))
		return arrs[0];
	return 0;

}

Optional<Media::MonitorSurface> Media::DDrawManager::CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth)
{
	if (this->IsError())
	{
		return 0;
	}
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(0);
	if (lpDD == 0)
	{
		return 0;
	}
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 surface;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_DEPTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = (UInt32)size.x;
	ddsd.dwHeight = (UInt32)size.y;
	ddsd.dwDepth = (UInt32)bitDepth;
	surface = 0;
	HRESULT res = lpDD->CreateSurface(&ddsd, &surface, NULL);
	if (res != DD_OK)
	{
		return 0;
	}
	Media::DDrawSurface *retSurface;
	NEW_CLASS(retSurface, Media::DDrawSurface(*this, lpDD, surface, 0, true, Media::RotateType::None));
	return retSurface;
}

Optional<Media::MonitorSurface> Media::DDrawManager::CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType)
{
	if (this->IsError())
	{
		return 0;
	}
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(hMon);
	if (lpDD == 0)
	{
		return 0;
	}
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	primarySurface = 0;
	HRESULT res = lpDD->CreateSurface(&ddsd, &primarySurface, NULL);
	if (res == DDERR_NOCOOPERATIVELEVELSET)
	{
		lpDD->SetCooperativeLevel((HWND)0, DDSCL_NORMAL);
		res = lpDD->CreateSurface(&ddsd, &primarySurface, NULL);
	}
	if (res != DD_OK)
	{
		return 0;
	}
	Media::DDrawSurface *surface;
	NEW_CLASS(surface, Media::DDrawSurface(*this, lpDD, primarySurface, hMon, true, rotateType));
	if (clipWindow.NotNull())
		surface->SetClipWindow(clipWindow);
	return surface;
}

Bool Media::DDrawManager::CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, RotateType rotateType)
{
	if (this->IsError())
	{
		return false;
	}
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->GetDD7(hMon);
	if (lpDD == 0)
	{
		return false;
	}
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 surface1;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 2;
	HRESULT res = lpDD->CreateSurface(&ddsd, &surface1, NULL);
	if (res != DD_OK)
	{
		return false;
	}
	LPDIRECTDRAWSURFACE7 surface2;
	DDSCAPS2 ddscaps;
	ZeroMemory(&ddscaps, sizeof(ddscaps));
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	surface1->GetAttachedSurface(&ddscaps, &surface2);

	NN<Media::DDrawSurface> ddSurface1;
	NN<Media::DDrawSurface> ddSurface2;
	NEW_CLASSNN(ddSurface1, Media::DDrawSurface(*this, lpDD, surface1, hMon, true, rotateType));
	NEW_CLASSNN(ddSurface2, Media::DDrawSurface(*this, lpDD, surface2, hMon, false, rotateType));
	ddSurface1->SetBuffSurface(ddSurface2);
	primarySurface.Set(ddSurface1);
	bufferSurface.Set(ddSurface2);
	return true;
}
