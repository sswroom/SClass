#ifndef _SM_MEDIA_DECODER_VDECODERCHAIN
#define _SM_MEDIA_DECODER_VDECODERCHAIN
#include "Data/ArrayList.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VDecoderChain : public VDecoderBase
		{
		private:
			Data::ArrayList<Media::IVideoSource *> *srcFilters;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VDecoderChain(IVideoSource *sourceVideo);
			virtual ~VDecoderChain();

			virtual Text::CString GetFilterName();

			void AddDecoder(Media::IVideoSource *decoder);
			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
		};
	}
}
#endif
