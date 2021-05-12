#ifndef _SM_MEDIA_DECODER_RAVCDECODER
#define _SM_MEDIA_DECODER_RAVCDECODER
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
			Bool toRelease;
			Bool size32;
			Bool firstFrame;
			Bool discontTime;
			UInt8 *readBuff;
			Media::H264Parser::H264Flags h264Flags;
			Bool skipHeader;

			Bool finfoMode;
			void *finfoData;
			Media::IVideoSource::FrameInfoCallback finfoCb;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			RAVCDecoder(IVideoSource *sourceVideo, Bool toRelease, Bool skipHeader);
			virtual ~RAVCDecoder();

			virtual const UTF8Char *GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);
			virtual UOSInt GetFrameSize(UOSInt frameIndex);
			virtual UOSInt ReadFrame(UOSInt frameIndex, UInt8 *buff);

			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		private:
			UOSInt BuildIFrameHeader(UInt8 *buff);
		};
	}
}
#endif
