#ifndef _SM_MEDIA_DECODER_VP09DECODER
#define _SM_MEDIA_DECODER_VP09DECODER
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
				UOSInt srcFrameIndex;
				UOSInt frameOfst;
				UOSInt frameSize;
				UOSInt fullFrameSize;
			} VP9FrameInfo;

			Data::ArrayList<VP9FrameInfo*> frameList;
			Bool finfoMode;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VP09Decoder(NN<VideoSource> sourceVideo, Bool toRelease);
			virtual ~VP09Decoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
			virtual UOSInt GetFrameSize(UOSInt frameIndex);
			virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		};
	}
}
#endif
