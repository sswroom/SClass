#ifndef _SM_MEDIA_VIDEOCHECKER
#define _SM_MEDIA_VIDEOCHECKER
#include "Media/IAudioSource.h"
#include "Media/IVideoSource.h"
#include "Media/MediaFile.h"
#include "Media/NullRenderer.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Sync/Event.h"

namespace Media
{
	class VideoChecker
	{
	private:
		typedef struct
		{
			UInt64 sampleCnt;
			Data::Duration lastSampleTime;
			Bool isEnd;
			Media::IAudioSource *adecoder;
			Media::NullRenderer *renderer;
			Media::IVideoSource *vdecoder;
			NotNullPtr<Sync::Event> evt;
		} DecodeStatus;
	private:
		Media::Decoder::VideoDecoderFinder vdecoders;
		Media::Decoder::AudioDecoderFinder adecoders;
		Bool allowTimeSkip;
		Sync::Event evt;

		static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		static void __stdcall OnVideoChange(Media::IVideoSource::FrameChange frChg, AnyType userData);
		static void __stdcall OnAudioEnd(AnyType userData);
	public:
		VideoChecker(Bool allowTimeSkip);
		~VideoChecker();

		void SetAllowTimeSkip(Bool allowTimeSkip);
		Bool IsValid(Media::MediaFile *mediaFile);
	};
}
#endif
