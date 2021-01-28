#ifndef _SM_MEDIA_REALTIMEVIDEOSOURCE
#define _SM_MEDIA_REALTIMEVIDEOSOURCE
#include "Media/IVideoSource.h"
#include "Media/IRealtimeVideoSource.h"
#include "Sync/Mutex.h"

namespace Media
{
	class RealtimeVideoSource : public IVideoSource
	{
	private:
		Media::IRealtimeVideoSource *capSrc;
		Sync::Mutex *frameMut;
		UInt8 *frameBuff;
		Int32 frameSize;
		UInt32 frameTime;
		UInt32 frameNum;
		Media::FrameType frameType;
		Int32 lastFrameTime;
		Int32 maxFrameSize;
		Media::FrameInfo frameInfo;
		Int32 frameRateNorm;
		Int32 frameRateDenorm;
		Bool started;

	private:
		static void __stdcall FrameCB(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UInt32 dataSize, Bool deltaFrame, void *userData, Media::FrameType frameType, Bool discontTime);

	public:
		RealtimeVideoSource(Media::IRealtimeVideoSource *capSrc);
		virtual ~RealtimeVideoSource();

		virtual WChar *GetName(WChar *buff);

		virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *rateNorm, Int32 *rateDenorm, Int32 *maxFrameSize);
		virtual void Start();
		virtual void Stop();
		virtual Int32 GetStreamTime(); //ms, -1 = infinity
		virtual Bool CanSeek();
		virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
		virtual Bool IsRealTimeSrc();

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
};
#endif
