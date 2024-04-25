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
			UOSInt maxFrameSize;
			Sync::Mutex frameMut;
			UInt8 *frameBuff;
			UOSInt frameSize;
			Bool lastIsField;
			Bool spsFound;
			UInt8 *sps;
			UOSInt spsSize;
			UInt8 *pps;
			UOSInt ppsSize;
			UInt8 *vps;
			UOSInt vpsSize;
			Bool toRelease;
			Bool firstFrame;
			Bool discontTime;

			Bool finfoMode;
			Data::CallbackStorage<Media::IVideoSource::FrameInfoCallback> finfoCb;

			static UOSInt CalcNALSize(const UInt8 *buff, UOSInt buffSize);
			static UInt8 *AppendNAL(UInt8 *outBuff, const UInt8 *srcBuff, UOSInt srcBuffSize);
			virtual void ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RHVCDecoder(NN<IVideoSource> sourceVideo, Bool toRelease);
			virtual ~RHVCDecoder();

			virtual Text::CStringNN GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual Data::Duration GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, AnyType userData);
			virtual UOSInt GetFrameSize(UOSInt frameIndex);
			virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);

			virtual Bool GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize);
		};
	}
}
#endif
