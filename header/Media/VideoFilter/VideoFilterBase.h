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
			Media::FrameInfo videoInfo;
			FrameCallback videoCb;
			FrameChangeCallback fcCb;
			AnyType userData;

			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange fc, AnyType userData);

			virtual void ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst) = 0;
			virtual void OnFrameChange(Media::IVideoSource::FrameChange fc);
		public:
			VideoFilterBase(Media::IVideoSource *srcVideo);
			virtual ~VideoFilterBase();

			virtual void SetSourceVideo(Media::IVideoSource *srcVideo);

			virtual UTF8Char *GetSourceName(UTF8Char *buff);

			virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom);
			virtual void GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom);
			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
			virtual Bool Start(); //true = succeed
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Data::Duration GetStreamTime();
			virtual Bool CanSeek();
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

			virtual UOSInt GetDataSeekCount();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype); //ret 0 = no more frames
		};
	}
}
#endif
