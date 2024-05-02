#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DDrawManager.h"
#include "Media/ScreenCapturer.h"

Media::ScreenCapturer::ScreenCapturer(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	NEW_CLASS(this->surfaceMgr, Media::DDrawManager(monMgr, colorMgr));
}

Media::ScreenCapturer::~ScreenCapturer()
{
	DEL_CLASS(this->surfaceMgr);
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(MonitorHandle *hMon)
{
	Media::MonitorSurface *surface = this->surfaceMgr->CreatePrimarySurface(hMon, 0, Media::RotateType::None);
	if (surface == 0) return 0;
	Media::StaticImage *retImg = surface->CreateStaticImage().Ptr();
	DEL_CLASS(surface);
	return retImg;
}
