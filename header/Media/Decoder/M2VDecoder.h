#ifndef _SM_MEDIA_DECODER_M2VDECODER
#define _SM_MEDIA_DECODER_M2VDECODER
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class M2VDecoder : public VDecoderBase
		{
		private:
			Bool toRelease;
			Double par;
			Bool discTime;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			M2VDecoder(NN<VideoSource> sourceVideo, Bool toRelease);
			virtual ~M2VDecoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
			virtual UIntOS GetFrameSize(UIntOS frameIndex);
			virtual UIntOS ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		};
	}
}
#endif
