#ifndef _SM_SSWR_AVIREAD_AVIRFCMFORM
#define _SM_SSWR_AVIREAD_AVIRFCMFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFCMForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblAPIKey;
			UI::GUITextBox *txtAPIKey;
			UI::GUILabel *lblDeviceToken;
			UI::GUITextBox *txtDeviceToken;
			UI::GUILabel *lblMessage;
			UI::GUITextBox *txtMessage;
			UI::GUIButton *btnSend;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;

			static void __stdcall OnSendClicked(void *userObj);
		public:
			AVIRFCMForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFCMForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
