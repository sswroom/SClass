#ifndef _SM_MEDIA_DECODER_VP09DECODER
#define _SM_MEDIA_DECODER_VP09DECODER
#include "Data/ArrayListNN.hpp"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VP09Decoder : public VDecoderBase
		{
		private:
			typedef struct
			{
				UIntOS srcFrameIndex;
				UIntOS frameOfst;
				UIntOS frameSize;
				UIntOS fullFrameSize;
			} VP9FrameInfo;

			Data::ArrayListNN<VP9FrameInfo> frameList;
			Bool finfoMode;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VP09Decoder(NN<VideoSource> sourceVideo, Bool toRelease);
			virtual ~VP09Decoder();

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
