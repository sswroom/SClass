#ifndef _SM_MEDIA_VIDEORENDERER
#define _SM_MEDIA_VIDEORENDERER
#include "Media/IVideoSource.h"

namespace Media
{
	class VideoRenderer
	{
/*	protected:
		Sync::Event *dispEvt;
		Bool dispToStop;
		Bool dispRunning;
		Bool dispForceUpdate;
		Sync::RWMutex *dispMut;
		Media::RefClock *dispClk;

		Bool playing;*/
	public:
		VideoRenderer();
		virtual ~VideoRenderer();

/*		void SetVideo(Media::IVideoSource *video);
		void SetHasAudio(Bool hasAudio);
		void SetTimeDelay(Int32 timeDelay);
		void VideoInit(Media::RefClock *clk);
		void VideoStart();
		void StopPlay();*/
	};
}
#endif
