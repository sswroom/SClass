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
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblAPIKey;
			NotNullPtr<UI::GUITextBox> txtAPIKey;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::PushServer *svr;
			IO::LogTool log;

			static void __stdcall OnStartClicked(void *userObj);
		public:
			AVIRPushServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPushServerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
