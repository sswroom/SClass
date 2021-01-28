#ifndef _SM_MEDIA_IVIDEORENDERER
#define _SM_MEDIA_IVIDEORENDERER
#include "Media/IVideoSource.h"
#include "Media/RefClock.h"

namespace Media
{
	class IVideoRenderer
	{
	public:
		typedef enum
		{
			RR_SUCCEED = 0,
			RR_NO_STATUS_CHG,
			RR_CLOCK_NOT_RUNNING,
			RR_NO_VIDEO
		} RendererResult;
	public:
		virtual ~IVideoRenderer(){};

		virtual Bool BindVideo(Media::IVideoSource *vidsrc) = 0;
		virtual RendererResult Start(Media::RefClock *clk) = 0;
		virtual void Stop() = 0;
		virtual Bool IsPlaying() = 0;
	};
};
#endif
