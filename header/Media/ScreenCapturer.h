#ifndef _SM_MEDIA_SCREENCAPTURER
#define _SM_MEDIA_SCREENCAPTURER
#include "Handles.h"
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/StaticImage.h"

namespace Media
{
	class ScreenCapturer
	{
	private:
		struct ClassData;

		ClassData *clsData;
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;

	public:
		ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		~ScreenCapturer();

		Media::StaticImage *CaptureScreen(MonitorHandle *hMon);
	};
}
#endif