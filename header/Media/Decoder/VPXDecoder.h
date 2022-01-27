#ifndef _SM_MEDIA_DECODER_VPXDECODER
#define _SM_MEDIA_DECODER_VPXDECODER
#include "Data/ArrayListInt32.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VPXDecoder : public VDecoderBase
		{
		private:
			void *clsData;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VPXDecoder(IVideoSource *sourceVideo);
			virtual ~VPXDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);
			virtual Text::CString GetFilterName();

			virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);

			Bool IsError();

			static void Enable();
		};
	}
}
#endif
