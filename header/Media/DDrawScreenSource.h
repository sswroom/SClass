#ifndef _SM_MEDIA_DDRAWSCREENSOURCE
#define _SM_MEDIA_DDRAWSCREENSOURCE
#include "Media/RealtimeVideoSource.h"

namespace Media
{
	class DDrawScreenSource : public Media::RealtimeVideoSource
	{
	private:
		Bool captureRunning;
		Bool captureToStop;
		Sync::Event *captureEvt;
		FrameCallback captureCb;
		void *captureCbData;
		UInt8 *scnBuff;
		UInt8 *scnABuff;
		OSInt scnBuffSize;

		void *ddObj;
		void *primarySurface;

		static UInt32 __stdcall CaptureThread(void *userObj);
		void CreateSurface();
		void ReleaseSurface();
	public:
		DDrawScreenSource();
		virtual ~DDrawScreenSource();

		virtual WChar *GetName(WChar *buff);

		virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, Int32 *maxFrameSize);
		virtual Bool Start(FrameCallback cb, void *userData);
		virtual void Stop();
		virtual Bool IsRunning();
	};
};
#endif