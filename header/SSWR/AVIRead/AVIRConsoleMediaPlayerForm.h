#ifndef _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#define _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#include "Media/ConsoleMediaPlayer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRConsoleMediaPlayerForm : public UI::GUIForm
		{
		private:
			UI::GUIButton *btnStop;

			SSWR::AVIRead::AVIRCore *core;
			Media::ConsoleMediaPlayer *player;

			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles);

		public:
			AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
