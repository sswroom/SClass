#ifndef _SM_SSWR_AVIREAD_AVIRHQMPDSFORM
#define _SM_SSWR_AVIREAD_AVIRHQMPDSFORM
#include "IO/Stream.h"
#include "Media/MediaFile.h"
#include "Media/Playlist.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMediaPlayer.h"
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
			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			SSWR::AVIRead::AVIRMediaPlayer *player;
			Media::MediaFile *currFile;
			Media::Playlist *playlist;
			Media::IPBControl *currPBC;
			Media::ChapterInfo *currChapInfo;
			Int32 uOfst;
			Int32 vOfst;
			Int32 storeTime;
			QualityMode qMode;

			UI::GUIVideoBoxDD *vbox;
			UI::GUIMainMenu *mnu;
			UI::GUIMenu *mnuChapters;

			UI::GUIForm *dbgFrm;
			UI::GUITextBox *txtDebug;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDebugClosed(void *userObj, UI::GUIForm *frm);
		public:
			Bool OpenFile(const UTF8Char *fileName);
			Bool OpenVideo(Media::MediaFile *mf);
		private:
			void SwitchAudio(OSInt audIndex);
			void CloseFile();

		public:
			AVIRHQMPDSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, QualityMode qMode);
			virtual ~AVIRHQMPDSForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif