#ifndef _SM_MEDIA_MEDIAPLAYER
#define _SM_MEDIA_MEDIAPLAYER
#include "Media/AudioDevice.h"
#include "Media/IMediaPlayer.h"
#include "Media/VideoRenderer.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"

namespace Media
{
	class MediaPlayer : public Media::IMediaPlayer
	{
	private:
		Optional<Media::AudioDevice> audioDev;
		Optional<Media::VideoRenderer> vrenderer;
		Media::Decoder::VideoDecoderFinder vdecoders;
		Media::Decoder::AudioDecoderFinder adecoders;
		Media::RefClock clk;

		Optional<Media::IAudioRenderer> arenderer;
		Media::IAudioSource *currADecoder;
		Optional<Media::MediaFile> currFile;
		Media::IAudioSource *currAStm;
		Media::IVideoSource *currVDecoder;
		Media::IVideoSource *currVStm;
		Media::ChapterInfo *currChapInfo;

		Bool playing;
		Bool videoPlaying;
		Bool audioPlaying;
		Data::Duration currTime;
		UOSInt pbLastChapter;

		PBEndHandler endHdlr;
		AnyType endObj;

		void PlayTime(Data::Duration time);
		static void __stdcall OnVideoEnd(AnyType userObj);
		static void __stdcall OnAudioEnd(AnyType userObj);
		static void __stdcall VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img);

		void ReleaseAudio();
		Bool SwitchAudioSource(NN<Media::IAudioSource> asrc, Int32 syncTime);
	public:
		MediaPlayer(NN<Media::VideoRenderer> vrenderer, Optional<Media::AudioDevice> audioDev);
		virtual ~MediaPlayer();

		virtual void SetEndHandler(PBEndHandler hdlr, AnyType userObj);
		virtual Bool LoadMedia(Optional<Media::MediaFile> file);
		virtual Bool SeekTo(Data::Duration time);
		virtual Bool SwitchAudio(UOSInt index);

		virtual Bool IsPlaying();
		virtual Bool StartPlayback();
		virtual Bool StopPlayback();
		virtual Bool PrevChapter();
		virtual Bool NextChapter();
		virtual Data::Duration GetCurrTime();

		virtual Bool GotoChapter(UOSInt chapter);
		virtual Bool GetVideoSize(OutParam<UOSInt> w, OutParam<UOSInt> h);
		virtual void DetectCrop();

		Optional<Media::VideoRenderer> GetVideoRenderer();
		void Close();
	};
}
#endif
