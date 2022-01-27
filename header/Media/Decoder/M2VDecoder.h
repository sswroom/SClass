#ifndef _SM_MEDIA_DECODER_M2VDECODER
#define _SM_MEDIA_DECODER_M2VDECODER
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class M2VDecoder : public VDecoderBase
		{
		private:
			Bool toRelease;
			Double par;
			Bool discTime;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			M2VDecoder(IVideoSource *sourceVideo, Bool toRelease);
			virtual ~M2VDecoder();

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
