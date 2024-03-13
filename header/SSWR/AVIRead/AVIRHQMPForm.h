#ifndef _SM_SSWR_AVIREAD_AVIRHQMPFORM
#define _SM_SSWR_AVIREAD_AVIRHQMPFORM
#include "IO/Stream.h"
#include "Media/MediaFile.h"
#include "Media/MediaPlayer.h"
#include "Media/MediaPlayerInterface.h"
#include "Media/Playlist.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/ImageFilter/BGImgFilter.h"
#include "Media/ImageFilter/BWImgFilter.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVideoBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHQMPForm : public UI::GUIForm, public Media::MediaPlayerInterface
		{
		public:
			typedef enum
			{
				QM_LQ,
				QM_HQ,
				QM_UQ
			} QualityMode;

		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Media::Playlist *playlist;
			Media::ChapterInfo *currChapInfo;
			Int32 uOfst;
			Int32 vOfst;
			QualityMode qMode;
			Bool pbEnd;
			Net::WebServer::WebListener *listener;

			Media::ImageFilter::BWImgFilter *bwFilter;
			Media::ImageFilter::BGImgFilter *bgFilter;

			UI::GUIVideoBoxDD *vbox;
			UI::GUIMainMenu *mnu;
			UI::GUIMenu *mnuChapters;

			UI::GUIForm *dbgFrm;
			NotNullPtr<UI::GUITextBox> txtDebug;

			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDebugClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnVideoEnd(void *userObj);
			static void __stdcall OnMouseAction(void *userObj, UI::GUIVideoBoxDD::MouseAction ma, Math::Coord2D<OSInt> scnPos);

			virtual void OnMediaOpened();
			virtual void OnMediaClosed();
		private:
			void SwitchAudio(OSInt audIndex);

		public:
			AVIRHQMPForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, QualityMode qMode);
			virtual ~AVIRHQMPForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual void DestroyObject();
		};
	}
}
#endif