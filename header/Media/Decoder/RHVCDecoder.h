#ifndef _SM_MEDIA_DECODER_RHVCDECODER
#define _SM_MEDIA_DECODER_RHVCDECODER
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
			Sync::Mutex *frameMut;
			UInt8 *frameBuff;
			UOSInt frameSize;
			Bool lastIsField;
			Bool lastFieldTime;
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
			void *finfoData;
			Media::IVideoSource::FrameInfoCallback finfoCb;

			static UOSInt CalcNALSize(const UInt8 *buff, UOSInt buffSize);
			static UInt8 *AppendNAL(UInt8 *outBuff, const UInt8 *srcBuff, UOSInt srcBuffSize);
			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RHVCDecoder(IVideoSource *sourceVideo, Bool toRelease);
			virtual ~RHVCDecoder();

			virtual Text::CString GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);
			virtual UOSInt GetFrameSize(UOSInt frameIndex);
			virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);

			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		};
	}
}
#endif
