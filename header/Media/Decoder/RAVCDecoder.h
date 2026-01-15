#ifndef _SM_MEDIA_DECODER_RAVCDECODER
#define _SM_MEDIA_DECODER_RAVCDECODER
#include "Data/CallbackStorage.h"
#include "Media/H264Parser.h"
#include "Media/Decoder/VDecoderBase.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Decoder
	{
		class RAVCDecoder : public VDecoderBase
		{
		private:
			UIntOS maxFrameSize;
			Sync::Mutex frameMut;
			UnsafeArray<UInt8> frameBuff;
			UIntOS frameSize;
			Bool lastIsField;
			Bool spsFound;
			UInt8 *sps;
			UIntOS spsSize;
			UInt8 *pps;
			UIntOS ppsSize;
			Bool toRelease;
			Bool size32;
			Bool firstFrame;
			Bool discontTime;
			Media::H264Parser::H264Flags h264Flags;
			Bool skipHeader;

			Bool finfoMode;
			Data::CallbackStorage<Media::VideoSource::FrameInfoCallback> finfoCb;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RAVCDecoder(NN<VideoSource> sourceVideo, Bool toRelease, Bool skipHeader);
			virtual ~RAVCDecoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual Data::Duration GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
			virtual UIntOS GetFrameSize(UIntOS frameIndex);
			virtual UIntOS ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize);
		private:
			UIntOS BuildIFrameHeader(UnsafeArray<UInt8> buff, Bool forceBuild);
		};
	}
}
#endif
