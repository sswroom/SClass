#ifndef _SM_SSWR_AVIREAD_AVIRHQMPFORM
#define _SM_SSWR_AVIREAD_AVIRHQMPFORM
#include "IO/Stream.h"
#include "Media/MediaFile.h"
#include "Media/Playlist.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/ImageFilter/BGImgFilter.h"
#include "Media/ImageFilter/BWImgFilter.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRMediaPlayer.h"
#include "UI/GUIForm.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVideoBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHQMPForm : public UI::GUIForm, public Net::WebServer::IWebHandler
		{
		public:
			typedef enum
			{
				QM_LQ,
				QM_HQ,
				QM_UQ
			} QualityMode;

		private:
			typedef struct
			{
				const UTF8Char *fileName;
				UInt64 fileSize;
			} VideoFileInfo;


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
			Bool pbEnd;
			Net::WebServer::WebListener *listener;

			Media::ImageFilter::BWImgFilter *bwFilter;
			Media::ImageFilter::BGImgFilter *bgFilter;

			UI::GUIVideoBoxDD *vbox;
			UI::GUIMainMenu *mnu;
			UI::GUIMenu *mnuChapters;

			UI::GUIForm *dbgFrm;
			UI::GUITextBox *txtDebug;

			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnDebugClosed(void *userObj, UI::GUIForm *frm);
			static void __stdcall OnVideoEnd(void *userObj);
			static void __stdcall OnMouseAction(void *userObj, UI::GUIVideoBoxDD::MouseAction ma, OSInt x, OSInt y);
		public:
			Bool OpenFile(const UTF8Char *fileName);
			Bool OpenVideo(Media::MediaFile *mf);
		private:
			void SwitchAudio(OSInt audIndex);
			void CloseFile();

			static OSInt __stdcall VideoFileCompare(void *file1, void *file2);

		public:
			AVIRHQMPForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, QualityMode qMode);
			virtual ~AVIRHQMPForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			void BrowseRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			virtual void WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			virtual void Release();

			void PBStart();
			void PBStop();
			void PBPause();
		};
	}
}
#endif