#ifndef _SM_MEDIA_VIDEOCHECKER
#define _SM_MEDIA_VIDEOCHECKER
#include "Media/AudioSource.h"
#include "Media/VideoSource.h"
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
			Optional<Media::AudioSource> adecoder;
			Media::NullRenderer *renderer;
			Optional<Media::VideoSource> vdecoder;
			NN<Sync::Event> evt;
		} DecodeStatus;
	private:
		Media::Decoder::VideoDecoderFinder vdecoders;
		Media::Decoder::AudioDecoderFinder adecoders;
		Bool allowTimeSkip;
		Sync::Event evt;

		static void __stdcall OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst);
		static void __stdcall OnVideoChange(Media::VideoSource::FrameChange frChg, AnyType userData);
		static void __stdcall OnAudioEnd(AnyType userData);
	public:
		VideoChecker(Bool allowTimeSkip);
		~VideoChecker();

		void SetAllowTimeSkip(Bool allowTimeSkip);
		Bool IsValid(NN<Media::MediaFile> mediaFile);
	};
}
#endif
