#ifndef _SM_MEDIA_DECODER_FFMPEGDECODER
#define _SM_MEDIA_DECODER_FFMPEGDECODER
#include "Data/ArrayListInt32.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class FFMPEGDecoder : public VDecoderBase
		{
		private:
			struct ClassData;

			ClassData *clsData;
			Bool endProcessing;
			Data::Duration lastFrameTime;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			FFMPEGDecoder(NN<IVideoSource> sourceVideo);
			virtual ~FFMPEGDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);
			virtual Text::CStringNN GetFilterName();

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);

			Bool IsError();

			static void Enable();
		};
	}
}
#endif
