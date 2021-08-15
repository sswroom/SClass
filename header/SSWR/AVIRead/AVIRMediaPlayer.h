#ifndef _SM_SSWR_AVIREAD_AVIRMEDIAPLAYER
#define _SM_SSWR_AVIREAD_AVIRMEDIAPLAYER
#include "Media/AudioDevice.h"
#include "Media/IMediaPlayer.h"
#include "Media/VideoRenderer.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRMediaPlayer.h"
#include "UI/GUIVideoBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMediaPlayer : public Media::IMediaPlayer
		{
		private:
			Media::AudioDevice *audioDev;
			UI::GUIVideoBoxDD *vrenderer;
			Media::Decoder::VideoDecoderFinder *vdecoders;
			Media::Decoder::AudioDecoderFinder *adecoders;
			Media::RefClock *clk;

			Media::IAudioRenderer *arenderer;
			Media::IAudioSource *currADecoder;
			Media::MediaFile *currFile;
			Media::IAudioSource *currAStm;
			Media::IVideoSource *currVDecoder;
			Media::IVideoSource *currVStm;
			Media::ChapterInfo *currChapInfo;

			Bool playing;
			Bool videoPlaying;
			Bool audioPlaying;
			UInt32 currTime;
			UOSInt pbLastChapter;

			PBEndHandler endHdlr;
			void *endObj;

			void PlayTime(UInt32 time);
			static void __stdcall OnVideoEnd(void *userObj);
			static void __stdcall OnAudioEnd(void *userObj);
			static void __stdcall VideoCropImage(void *userObj, UInt32 frameTime, UInt32 frameNum, Media::StaticImage *img);

			void ReleaseAudio();
			Bool SwitchAudioSource(Media::IAudioSource *asrc, Int32 syncTime);
		public:
			AVIRMediaPlayer(UI::GUIVideoBoxDD *vrenderer, Media::AudioDevice *audioDev);
			virtual ~AVIRMediaPlayer();

			virtual void SetEndHandler(PBEndHandler hdlr, void *userObj);
			virtual Bool LoadMedia(Media::MediaFile *file);
			virtual Bool SeekTo(UInt32 time);
			virtual Bool SwitchAudio(UOSInt index);

			virtual Bool IsPlaying();
			virtual Bool StartPlayback();
			virtual Bool StopPlayback();
			virtual Bool PrevChapter();
			virtual Bool NextChapter();
			virtual UInt32 GetCurrTime();

			virtual Bool GotoChapter(UOSInt chapter);
			virtual Bool GetVideoSize(UOSInt *w, UOSInt *h);
			virtual void DetectCrop();
		};
	}
}
#endif
