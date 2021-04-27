#ifndef _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#define _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/IAudioRenderer.h"
#include "Media/IAudioSource.h"
#include "Media/IVideoSource.h"
#include "Media/MediaFile.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUIVideoBoxDD.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMediaForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbFiles;
			UI::GUIPanel *pnlCtrl;
			UI::GUIHSplitter *hsplit;
			UI::GUIVideoBoxDD *vbdMain;
			UI::GUIMainMenu *mnu;
			UI::GUIMenu *mnuChapters;
			UI::GUIPopupMenu *mnuAudio;
			UI::GUIPopupMenu *mnuVideo;

			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<Media::MediaFile *> *files;
			Media::IMediaSource *popMedia;

			Media::Decoder::AudioDecoderFinder *adecoders;
			Media::Decoder::VideoDecoderFinder *decoders;
			Media::IAudioRenderer *audRenderer;
			Media::RefClock *clk;
			Media::IAudioSource *activeAudio;
			Media::IVideoSource *activeVideo;
			Media::IAudioSource *currADecoder;
			Media::IVideoSource *currDecoder;
			Media::ChapterInfo *currChapters;

			Bool playing;
			UOSInt pbLastChapter;

			void UpdateStreamList();
			void UpdateChapters();
			void SetActiveVideo(Media::IVideoSource *video);
			void SetActiveAudio(Media::IAudioSource *audio, Int32 timeDelay);
		private:
			static Bool __stdcall OnFileRClicked(void *userObj, OSInt scnX, OSInt scnY, UI::GUIControl::MouseButton btn);
			static void __stdcall OnFileDblClicked(void *userObj);
			static void __stdcall VideoCropImage(void *userObj, UInt32 frameTime, UInt32 frameNum, Media::StaticImage *img);
			static Bool __stdcall OnFrameTime(UInt32 frameTime, OSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, void *userData, Media::YCOffset ycOfst);

			void PBStart(Int32 startTime);
			void PBStop();
			Bool PBIsPlaying();
		public:
			AVIRMediaForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::MediaFile *mediaFile);
			virtual ~AVIRMediaForm();
		
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
