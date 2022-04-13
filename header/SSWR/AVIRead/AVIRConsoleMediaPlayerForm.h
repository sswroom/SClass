#ifndef _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#define _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#include "Media/ConsoleMediaPlayer.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRConsoleMediaPlayerForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStop;
			UI::GUIButton *btnCaptureDev;
			UI::GUILabel *lblRotate;
			UI::GUIComboBox *cboRotate;
			UI::GUICheckBox *chkSurfaceBug;

			SSWR::AVIRead::AVIRCore *core;
			Media::ConsoleMediaPlayer *player;
			Media::MediaPlayerWebInterface *webIface;
			Net::WebServer::WebListener *listener;

			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnCaptureDevClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnRotateChg(void *userObj);
			static void __stdcall OnSurfaceBugChg(void *userObj, Bool newVal);

		public:
			AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
