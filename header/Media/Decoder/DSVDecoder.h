#ifndef _SM_MEDIA_DECODER_DSVDECODER
#define _SM_MEDIA_DECODER_DSVDECODER
#include "Data/ArrayListInt32.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class DSVDecoder : public VDecoderBase
		{
		private:
			Int32 frameRateNorm;
			Int32 frameRateDenorm;
			Int32 maxFrameSize;
			void *bmihSrc;
			void *bmihDest;
			void *hic;
			UInt8 *frameBuff;
			void *graph;

			ImageCallback imgCb;
			void *imgCbData;
			Int32 sourceFCC;

			Media::YCOffset lastYCOfst;

			Bool endProcessing;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			DSVDecoder(IVideoSource *sourceAudio);
			virtual ~DSVDecoder();

			virtual Bool CaptureImage(ImageCallback imgCb, void *userData);

			virtual Bool GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize);
			virtual void Stop();

			virtual Bool HasFrameCount();
			virtual UOSInt GetFrameCount();
			virtual UInt32 GetFrameTime(UOSInt frameIndex);
			virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData);

			virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc);
		};
	}
}
#endif
