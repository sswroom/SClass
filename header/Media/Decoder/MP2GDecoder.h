#ifndef _SM_MEDIA_DECODER_MP2GDECODER
#define _SM_MEDIA_DECODER_MP2GDECODER
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
			void *finfoData;
			FrameInfoCallback finfoCb;

			Bool hasBFrame;
			UInt32 lastFrameTime;
			Media::FrameType lastFrameType;
			Media::IVideoSource::FrameFlag lastFrameFlags;
			Media::IVideoSource::FrameStruct lastFrameStruct;
			UInt32 lastFieldOfst;
			Bool lastRFF;


			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			MP2GDecoder(IVideoSource *sourceVideo, Bool toRelease);
			virtual ~MP2GDecoder();

			virtual Text::CString GetFilterName();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		};
	}
}
#endif
