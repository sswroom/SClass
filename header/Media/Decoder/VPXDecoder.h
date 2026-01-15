#ifndef _SM_MEDIA_DECODER_VPXDECODER
#define _SM_MEDIA_DECODER_VPXDECODER
#include "Data/ArrayListInt32.h"
#include "Media/VideoSource.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VPXDecoder : public VDecoderBase
		{
		private:
			void *clsData;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VPXDecoder(VideoSource *sourceVideo);
			virtual ~VPXDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);
			virtual Text::CString GetFilterName();

			virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UIntOS *maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UIntOS GetFrameCount();
			virtual UInt32 GetFrameTime(UIntOS frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void OnFrameChanged(Media::VideoSource::FrameChange fc);

			Bool IsError();

			static void Enable();
		};
	}
}
#endif
