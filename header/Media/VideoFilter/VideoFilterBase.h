#ifndef _SM_MEDIA_VIDEOFILTER_VIDEOFILTERBASE
#define _SM_MEDIA_VIDEOFILTER_VIDEOFILTERBASE
#include "Media/VideoSource.h"

namespace Media
{
	namespace VideoFilter
	{
		class VideoFilterBase : public Media::VideoSource
		{
		protected:
			Optional<Media::VideoSource> srcVideo;
			Media::FrameInfo videoInfo;
			FrameCallback videoCb;
			FrameChangeCallback fcCb;
			AnyType userData;

			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData);

			virtual void ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst) = 0;
			virtual void OnFrameChange(Media::VideoSource::FrameChange fc);
		public:
			VideoFilterBase(Optional<Media::VideoSource> srcVideo);
			virtual ~VideoFilterBase();

			virtual void SetSourceVideo(Optional<Media::VideoSource> srcVideo);

			virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);

			virtual void SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom);
			virtual void GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom);
			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
			virtual Bool Start(); //true = succeed
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Data::Duration GetStreamTime();
			virtual Bool CanSeek();
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

			virtual UIntOS GetDataSeekCount();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual UIntOS ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype); //ret 0 = no more frames
		};
	}
}
#endif
