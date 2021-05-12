#ifndef _SM_MEDIA_DECODER_VP09DECODER
#define _SM_MEDIA_DECODER_VP09DECODER
#include "Media/Decoder/VDecoderBase.h"

namespace Media
{
	namespace Decoder
	{
		class VP09Decoder : public VDecoderBase
		{
		private:
			typedef struct
			{
				UOSInt srcFrameIndex;
				UOSInt frameOfst;
				UOSInt frameSize;
				UOSInt fullFrameSize;
			} VP9FrameInfo;

			Data::ArrayList<VP9FrameInfo*> *frameList;
			Bool finfoMode;

			virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		public:
			VP09Decoder(IVideoSource *sourceVideo, Bool toRelease);
			virtual ~VP09Decoder();

			virtual const UTF8Char *GetFilterName();

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
