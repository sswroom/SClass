#ifndef _SM_MEDIA_DECODER_VDECODERCHAIN
#define _SM_MEDIA_DECODER_VDECODERCHAIN
#include "Data/ArrayList.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VDecoderChain : public VDecoderBase
		{
		private:
			Data::ArrayList<Media::IVideoSource *> srcFilters;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VDecoderChain(NotNullPtr<IVideoSource> sourceVideo);
			virtual ~VDecoderChain();

			virtual Text::CStringNN GetFilterName();

			void AddDecoder(NotNullPtr<Media::IVideoSource> decoder);
			virtual Bool CaptureImage(ImageCallback imgCb, AnyType userData);

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual Bool GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		};
	}
}
#endif
