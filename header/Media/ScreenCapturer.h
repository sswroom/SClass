#ifndef _SM_MEDIA_SCREENCAPTURER
#define _SM_MEDIA_SCREENCAPTURER
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/StaticImage.h"

namespace Media
{
	class ScreenCapturer
	{
	private:
		void *clsData;
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;

	public:
		ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		~ScreenCapturer();

		Media::StaticImage *CaptureScreen(void *hMon);
	};
};
#endif