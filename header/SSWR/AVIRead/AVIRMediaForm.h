#ifndef _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#define _SM_SSWR_AVIREAD_AVIRMEDIAFORM
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/AudioRenderer.h"
#include "Media/AudioSource.h"
#include "Media/VideoSource.h"
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
			NN<UI::GUIListBox> lbFiles;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIHSplitter> hsplit;
			NN<UI::GUIVideoBoxDD> vbdMain;
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUIMenu> mnuChapters;
			NN<UI::GUIPopupMenu> mnuAudio;
			NN<UI::GUIPopupMenu> mnuVideo;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Data::ArrayListNN<Media::MediaFile> files;
			Optional<Media::MediaSource> popMedia;

			NN<Media::Decoder::AudioDecoderFinder> adecoders;
			NN<Media::Decoder::VideoDecoderFinder> decoders;
			Optional<Media::AudioRenderer> audRenderer;
			NN<Media::RefClock> clk;
			Optional<Media::AudioSource> activeAudio;
			Optional<Media::VideoSource> activeVideo;
			Optional<Media::AudioSource> currADecoder;
			Optional<Media::VideoSource> currDecoder;
			Optional<Media::ChapterInfo> currChapters;

			Bool playing;
			UIntOS pbLastChapter;

			void UpdateStreamList();
			void UpdateChapters();
			void SetActiveVideo(NN<Media::VideoSource> video);
			void SetActiveAudio(NN<Media::AudioSource> audio, Int32 timeDelay);
		private:
			static UI::EventState __stdcall OnFileRClicked(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall OnFileDblClicked(AnyType userObj);
			static void __stdcall VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img);
			static Bool __stdcall OnFrameTime(Data::Duration frameTime, UIntOS frameNum, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst);

			void PBStart(Data::Duration startTime);
			void PBStop();
			Bool PBIsPlaying();
		public:
			AVIRMediaForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::MediaFile> mediaFile);
			virtual ~AVIRMediaForm();
		
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
