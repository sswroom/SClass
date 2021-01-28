#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ScreenCapturer.h"

Media::ScreenCapturer::ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
}

Media::ScreenCapturer::~ScreenCapturer()
{
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(void *hMon)
{
	return 0;
}
