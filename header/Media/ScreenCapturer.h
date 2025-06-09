#ifndef _SM_MEDIA_SCREENCAPTURER
#define _SM_MEDIA_SCREENCAPTURER
#include "Handles.h"
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"
#include "Media/StaticImage.h"

namespace Media
{
	class ScreenCapturer
	{
	private:
		Media::MonitorSurfaceMgr *surfaceMgr;
		NN<Media::MonitorMgr> monMgr;
		NN<Media::ColorManager> colorMgr;

	public:
		ScreenCapturer(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr);
		~ScreenCapturer();

		Optional<Media::StaticImage> CaptureScreen(Optional<MonitorHandle> hMon);
	};
}
#endif