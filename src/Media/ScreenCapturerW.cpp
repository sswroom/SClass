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
	Media::DDrawManager *ddMgr;
	NEW_CLASS(ddMgr, Media::DDrawManager(monMgr, colorMgr));
	this->clsData = (ClassData*)ddMgr;
}

Media::ScreenCapturer::~ScreenCapturer()
{
	Media::DDrawManager *ddMgr = (Media::DDrawManager*)this->clsData;
	DEL_CLASS(ddMgr);
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(MonitorHandle *hMon)
{
	Media::DDrawManager *ddMgr = (Media::DDrawManager*)this->clsData;
	Media::MonitorSurface *surface = ddMgr->CreatePrimarySurface(hMon, 0);
	if (surface == 0) return 0;
	Media::StaticImage *retImg = surface->CreateStaticImage();
	DEL_CLASS(surface);
	return retImg;
}
