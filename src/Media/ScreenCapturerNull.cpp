#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ScreenCapturer.h"

Media::ScreenCapturer::ScreenCapturer(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
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
