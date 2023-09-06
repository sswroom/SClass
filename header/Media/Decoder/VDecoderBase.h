#ifndef _SM_MEDIA_DECODER_VDECODERBASE
#define _SM_MEDIA_DECODER_VDECODERBASE
#include "Media/IVideoSource.h"

namespace Media
{
	namespace Decoder
	{
		class VDecoderBase : public IVideoSource
		{
		protected:
			Media::IVideoSource *sourceVideo;
			FrameCallback frameCb;
			FrameChangeCallback fcCb;
			void *frameCbData;
			Bool started;

			static void __stdcall OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
			static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange fc, void *userData);

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst) = 0;
		public:
			VDecoderBase(IVideoSource *sourceVideo);
			virtual ~VDecoderBase();

			virtual void SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom);
			virtual void GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom);

			virtual UTF8Char *GetSourceName(UTF8Char *buff);

			virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize) = 0;

			virtual Bool Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData);
			virtual Bool Start();
			virtual void Stop();
			virtual Bool IsRunning();

			virtual Int32 GetStreamTime();
			virtual Bool CanSeek();
			virtual UInt32 SeekToTime(UInt32 time);
			virtual Bool IsRealTimeSrc();
			virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

			virtual UOSInt GetDataSeekCount();

			virtual UOSInt ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype);
			virtual UInt8 *GetProp(Int32 propName, UInt32 *size);
			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);
		};
	}
}
#endif
