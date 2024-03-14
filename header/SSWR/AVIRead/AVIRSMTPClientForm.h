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
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblSSLType;
			NotNullPtr<UI::GUIComboBox> cboSSLType;
			NotNullPtr<UI::GUILabel> lblUsername;
			NotNullPtr<UI::GUITextBox> txtUsername;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUILabel> lblFromAddr;
			NotNullPtr<UI::GUITextBox> txtFromAddr;
			NotNullPtr<UI::GUILabel> lblToAddr;
			NotNullPtr<UI::GUITextBox> txtToAddr;
			NotNullPtr<UI::GUILabel> lblCcAddr;
			NotNullPtr<UI::GUITextBox> txtCcAddr;
			NotNullPtr<UI::GUILabel> lblBccAddr;
			NotNullPtr<UI::GUITextBox> txtBccAddr;
			NotNullPtr<UI::GUILabel> lblSubject;
			NotNullPtr<UI::GUITextBox> txtSubject;
			NotNullPtr<UI::GUILabel> lblContent;
			NotNullPtr<UI::GUITextBox> txtContent;
			NotNullPtr<UI::GUIButton> btnSend;
			NotNullPtr<UI::GUITextBox> txtLog;

			static void __stdcall OnSendClicked(AnyType userObj);
		public:
			AVIRSMTPClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSMTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
