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
			UOSInt maxFrameSize;
			Sync::Mutex frameMut;
			UnsafeArray<UInt8> frameBuff;
			UOSInt frameSize;
			Bool lastIsField;
			Bool spsFound;
			UInt8 *sps;
			UOSInt spsSize;
			UInt8 *pps;
			UOSInt ppsSize;
			Bool toRelease;
			Bool size32;
			Bool firstFrame;
			Bool discontTime;
			Media::H264Parser::H264Flags h264Flags;
			Bool skipHeader;

			Bool finfoMode;
			Data::CallbackStorage<Media::IVideoSource::FrameInfoCallback> finfoCb;

			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RAVCDecoder(NN<IVideoSource> sourceVideo, Bool toRelease, Bool skipHeader);
			virtual ~RAVCDecoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
			virtual UOSInt GetFrameSize(UOSInt frameIndex);
			virtual UOSInt ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		private:
			UOSInt BuildIFrameHeader(UnsafeArray<UInt8> buff, Bool forceBuild);
		};
	}
}
#endif
