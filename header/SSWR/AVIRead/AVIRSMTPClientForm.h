#ifndef _SM_SSWR_AVIREAD_AVIRSMTPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRSMTPCLIENTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSMTPClientForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblHost;
			UI::GUITextBox *txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			UI::GUITextBox *txtPort;
			NotNullPtr<UI::GUILabel> lblSSLType;
			NotNullPtr<UI::GUIComboBox> cboSSLType;
			NotNullPtr<UI::GUILabel> lblUsername;
			UI::GUITextBox *txtUsername;
			NotNullPtr<UI::GUILabel> lblPassword;
			UI::GUITextBox *txtPassword;
			NotNullPtr<UI::GUILabel> lblFromAddr;
			UI::GUITextBox *txtFromAddr;
			NotNullPtr<UI::GUILabel> lblToAddr;
			UI::GUITextBox *txtToAddr;
			NotNullPtr<UI::GUILabel> lblCcAddr;
			UI::GUITextBox *txtCcAddr;
			NotNullPtr<UI::GUILabel> lblBccAddr;
			UI::GUITextBox *txtBccAddr;
			NotNullPtr<UI::GUILabel> lblSubject;
			UI::GUITextBox *txtSubject;
			NotNullPtr<UI::GUILabel> lblContent;
			UI::GUITextBox *txtContent;
			NotNullPtr<UI::GUIButton> btnSend;
			UI::GUITextBox *txtLog;

			static void __stdcall OnSendClicked(void *userObj);
		public:
			AVIRSMTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSMTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
