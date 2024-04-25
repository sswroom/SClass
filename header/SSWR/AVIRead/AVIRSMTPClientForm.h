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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblSSLType;
			NN<UI::GUIComboBox> cboSSLType;
			NN<UI::GUILabel> lblUsername;
			NN<UI::GUITextBox> txtUsername;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUILabel> lblFromAddr;
			NN<UI::GUITextBox> txtFromAddr;
			NN<UI::GUILabel> lblToAddr;
			NN<UI::GUITextBox> txtToAddr;
			NN<UI::GUILabel> lblCcAddr;
			NN<UI::GUITextBox> txtCcAddr;
			NN<UI::GUILabel> lblBccAddr;
			NN<UI::GUITextBox> txtBccAddr;
			NN<UI::GUILabel> lblSubject;
			NN<UI::GUITextBox> txtSubject;
			NN<UI::GUILabel> lblContent;
			NN<UI::GUITextBox> txtContent;
			NN<UI::GUIButton> btnSend;
			NN<UI::GUITextBox> txtLog;

			static void __stdcall OnSendClicked(AnyType userObj);
		public:
			AVIRSMTPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSMTPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
