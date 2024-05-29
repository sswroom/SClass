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

Optional<Media::StaticImage> Media::ScreenCapturer::CaptureScreen(MonitorHandle *hMon)
{
	NN<Media::MonitorSurface> surface;
	if (!this->surfaceMgr->CreatePrimarySurface(hMon, 0, Media::RotateType::None).SetTo(surface)) return 0;
	NN<Media::StaticImage> retImg = surface->CreateStaticImage();
	surface.Delete();
	return retImg;
}
