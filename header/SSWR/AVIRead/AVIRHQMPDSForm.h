#ifndef _SM_SSWR_AVIREAD_AVIRHQMPDSFORM
#define _SM_SSWR_AVIREAD_AVIRHQMPDSFORM
#include "IO/Stream.h"
#include "Media/MediaFile.h"
#include "Media/MediaPlayer.h"
#include "Media/Playlist.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIVideoBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHQMPDSForm : public UI::GUIForm
		{
		public:
			typedef enum
			{
				QM_LQ,
				QM_HQ,
				QM_UQ
			} QualityMode;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::MediaPlayer> player;
			Optional<Media::MediaFile> currFile;
			Optional<Media::Playlist> playlist;
			NN<Media::PBControl> currPBC;
			Optional<Media::ChapterInfo> currChapInfo;
			Int32 uOfst;
			Int32 vOfst;
			Data::Duration storeTime;
			QualityMode qMode;

			NN<UI::GUIVideoBoxDD> vbox;
			NN<UI::GUIMainMenu> mnu;
			NN<UI::GUIMenu> mnuChapters;

			Optional<UI::GUIForm> dbgFrm;
			NN<UI::GUITextBox> txtDebug;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnDebugClosed(AnyType userObj, NN<UI::GUIForm> frm);
		public:
			Bool OpenFile(Text::CStringNN fileName, IO::ParserType targetType);
			Bool OpenVideo(NN<Media::MediaFile> mf);
		private:
			void SwitchAudio(UOSInt audIndex);
			void CloseFile();

		public:
			AVIRHQMPDSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, QualityMode qMode);
			virtual ~AVIRHQMPDSForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif