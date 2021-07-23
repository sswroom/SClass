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

			virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom);
			virtual void GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom);
			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
			virtual Bool Start(); //true = succeed
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Int32 GetStreamTime(); //ms, -1 = infinity
			virtual Bool CanSeek();
			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

			virtual UOSInt GetDataSeekCount();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
		};
	}
}
#endif
