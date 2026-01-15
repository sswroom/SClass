#ifndef _SM_MEDIA_DECODER_RHVCDECODER
#define _SM_MEDIA_DECODER_RHVCDECODER
#include "Data/CallbackStorage.h"
#include "Media/H264Parser.h"
#include "Media/Decoder/VDecoderBase.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Decoder
	{
		class RHVCDecoder : public VDecoderBase
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
			UInt8 *vps;
			UIntOS vpsSize;
			Bool toRelease;
			Bool firstFrame;
			Bool discontTime;

			Bool finfoMode;
			Data::CallbackStorage<Media::VideoSource::FrameInfoCallback> finfoCb;

			static UIntOS CalcNALSize(const UInt8 *buff, UIntOS buffSize);
			static UInt8 *AppendNAL(UInt8 *outBuff, const UInt8 *srcBuff, UIntOS srcBuffSize);
			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RHVCDecoder(NN<VideoSource> sourceVideo, Bool toRelease);
			virtual ~RHVCDecoder();

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
