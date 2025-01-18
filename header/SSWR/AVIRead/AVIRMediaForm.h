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
			Optional<Media::IMediaSource> popMedia;

			NN<Media::Decoder::AudioDecoderFinder> adecoders;
			NN<Media::Decoder::VideoDecoderFinder> decoders;
			Optional<Media::IAudioRenderer> audRenderer;
			NN<Media::RefClock> clk;
			Optional<Media::IAudioSource> activeAudio;
			Optional<Media::IVideoSource> activeVideo;
			Optional<Media::IAudioSource> currADecoder;
			Optional<Media::IVideoSource> currDecoder;
			Optional<Media::ChapterInfo> currChapters;

			Bool playing;
			UOSInt pbLastChapter;

			void UpdateStreamList();
			void UpdateChapters();
			void SetActiveVideo(NN<Media::IVideoSource> video);
			void SetActiveAudio(NN<Media::IAudioSource> audio, Int32 timeDelay);
		private:
			static Bool __stdcall OnFileRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn);
			static void __stdcall OnFileDblClicked(AnyType userObj);
			static void __stdcall VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img);
			static Bool __stdcall OnFrameTime(Data::Duration frameTime, UOSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst);

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
