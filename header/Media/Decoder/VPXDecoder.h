#ifndef _SM_MEDIA_DECODER_VPXDECODER
#define _SM_MEDIA_DECODER_VPXDECODER
#include "Data/ArrayListInt32.h"
#include "Media/VideoSource.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VPXDecoder : public VDecoderBase
		{
		private:
			struct ClassData;
			NN<ClassData> clsData;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VPXDecoder(NN<VideoSource> sourceVideo);
			virtual ~VPXDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual void OnFrameChanged(Media::VideoSource::FrameChange fc);

			Bool IsError();

			static void Enable();
		};
	}
}
#endif
