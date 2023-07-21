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
			SSWR::AVIRead::AVIRCore *core;
			Net::SSLEngine *ssl;

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblSSLType;
			UI::GUIComboBox *cboSSLType;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUILabel *lblFromAddr;
			UI::GUITextBox *txtFromAddr;
			UI::GUILabel *lblToAddr;
			UI::GUITextBox *txtToAddr;
			UI::GUILabel *lblCcAddr;
			UI::GUITextBox *txtCcAddr;
			UI::GUILabel *lblBccAddr;
			UI::GUITextBox *txtBccAddr;
			UI::GUILabel *lblSubject;
			UI::GUITextBox *txtSubject;
			UI::GUILabel *lblContent;
			UI::GUITextBox *txtContent;
			UI::GUIButton *btnSend;
			UI::GUITextBox *txtLog;

			static void __stdcall OnSendClicked(void *userObj);
		public:
			AVIRSMTPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRSMTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
