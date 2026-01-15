#ifndef _SM_MEDIA_DECODER_VDECODERBASE
#define _SM_MEDIA_DECODER_VDECODERBASE
#include "Media/VideoSource.h"

namespace Media
{
	namespace Decoder
	{
		class VDecoderBase : public VideoSource
		{
		protected:
			Media::VideoSource *sourceVideo;
			FrameCallback frameCb;
			FrameChangeCallback fcCb;
			AnyType frameCbData;
			Bool started;

			static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData);

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst) = 0;
		public:
			VDecoderBase(NN<VideoSource> sourceVideo);
			virtual ~VDecoderBase();

			virtual void SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom);
			virtual void GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom);

			virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize) = 0;

			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData);
			virtual Bool Start();
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Data::Duration GetStreamTime();
			virtual Bool CanSeek();
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

			virtual UIntOS GetDataSeekCount();

			virtual UIntOS ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype);
			virtual UInt8 *GetProp(Int32 propName, UInt32 *size);
			virtual void OnFrameChanged(Media::VideoSource::FrameChange fc);
		};
	}
}
#endif
