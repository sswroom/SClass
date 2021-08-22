#ifndef _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#define _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#include "Media/ConsoleMediaPlayer.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
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
			UI::GUILabel *lblRotate;
			UI::GUIComboBox *cboRotate;

			SSWR::AVIRead::AVIRCore *core;
			Media::ConsoleMediaPlayer *player;
			Media::MediaPlayerWebInterface *webIface;
			Net::WebServer::WebListener *listener;

			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnRotateChg(void *userObj);

		public:
			AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
