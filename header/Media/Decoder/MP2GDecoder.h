#ifndef _SM_MEDIA_DECODER_MP2GDECODER
#define _SM_MEDIA_DECODER_MP2GDECODER
#include "Data/CallbackStorage.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class MP2GDecoder : public VDecoderBase
		{
		private:
			UInt32 frameRateNorm;
			UInt32 frameRateDenorm;
			Double par;
			Bool toRelease;

			Bool finfoMode;
			Data::CallbackStorage<FrameInfoCallback> finfoCb;

			Bool hasBFrame;
			Data::Duration lastFrameTime;
			Media::FrameType lastFrameType;
			Media::VideoSource::FrameFlag lastFrameFlags;
			Media::VideoSource::FrameStruct lastFrameStruct;
			UInt32 lastFieldOfst;
			Bool lastRFF;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			MP2GDecoder(NN<VideoSource> sourceVideo, Bool toRelease);
			virtual ~MP2GDecoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		};
	}
}
#endif
