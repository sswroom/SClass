#ifndef _SM_SSWR_AVIREAD_AVIRPUSHSERVERFORM
#define _SM_SSWR_AVIREAD_AVIRPUSHSERVERFORM
#include "Net/PushServer.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPushServerForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblAPIKey;
			UI::GUITextBox *txtAPIKey;
			UI::GUIButton *btnStart;

			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;
			Net::PushServer *svr;
			IO::LogTool *log;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRPushServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRPushServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif