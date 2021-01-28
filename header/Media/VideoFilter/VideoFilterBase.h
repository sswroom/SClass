#ifndef _SM_MEDIA_VIDEOFILTER_VIDEOFILTERBASE
#define _SM_MEDIA_VIDEOFILTER_VIDEOFILTERBASE
#include "Media/IVideoSource.h"

namespace Media
{
	namespace VideoFilter
	{
		class VideoFilterBase : public Media::IVideoSource
		{
		protected:
			Media::IVideoSource *srcVideo;
			Media::FrameInfo *videoInfo;
			FrameCallback videoCb;
			FrameChangeCallback fcCb;
			void *userData;

			static void __stdcall OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange fc, void *userData);

			virtual void ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst) = 0;
			virtual void OnFrameChange(Media::IVideoSource::FrameChange fc);
		public:
			VideoFilterBase(Media::IVideoSource *srcVideo);
			virtual ~VideoFilterBase();

			virtual void SetSourceVideo(Media::IVideoSource *srcVideo);

			virtual UTF8Char *GetSourceName(UTF8Char *buff);

			virtual void SetBorderCrop(OSInt cropLeft, OSInt cropTop, OSInt cropRight, OSInt cropBottom);
			virtual void GetBorderCrop(OSInt *cropLeft, OSInt *cropTop, OSInt *cropRight, OSInt *cropBottom);
			virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
			virtual Bool Start(); //true = succeed
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Int32 GetStreamTime(); //ms, -1 = infinity
			virtual Bool CanSeek();
			virtual Int32 SeekToTime(Int32 time); //ms, ret actual time
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime);

			virtual OSInt GetDataSeekCount();

			virtual OSInt GetFrameCount(); //-1 = unknown;
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual OSInt ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
		};
	}
}
#endif
