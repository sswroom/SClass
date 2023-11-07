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
		Sync::Mutex frameMut;
		UInt8 *frameBuff;
		Int32 frameSize;
		UInt32 frameTime;
		UInt32 frameNum;
		Media::FrameType frameType;
		Int32 lastFrameTime;
		UOSInt maxFrameSize;
		Media::FrameInfo frameInfo;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		Bool started;

	private:
		static void __stdcall FrameCB(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
	public:
		RealtimeVideoSource(Media::IRealtimeVideoSource *capSrc);
		virtual ~RealtimeVideoSource();

		virtual UTF8Char *GetName(UTF8Char *buff);

		virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *rateNorm, UInt32 *rateDenorm, UOSInt *maxFrameSize);
		virtual Bool Start();
		virtual void Stop();
		virtual Data::Duration GetStreamTime();
		virtual Bool CanSeek();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool IsRealTimeSrc();

		virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
	};
}
#endif
