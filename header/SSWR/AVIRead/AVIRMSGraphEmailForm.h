#ifndef _SM_SSWR_AVIREAD_AVIRMSGRAPHEMAILFORM
#define _SM_SSWR_AVIREAD_AVIRMSGRAPHEMAILFORM
#include "Net/MSGraphClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMSGraphEmailForm : public UI::GUIForm
		{
		private:
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblTenantId;
			NN<UI::GUITextBox> txtTenantId;
			NN<UI::GUILabel> lblClientId;
			NN<UI::GUITextBox> txtClientId;
			NN<UI::GUILabel> lblClientSecret;
			NN<UI::GUITextBox> txtClientSecret;
			NN<UI::GUILabel> lblUserName;
			NN<UI::GUITextBox> txtUserName;
			NN<UI::GUIButton> btnRead;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpMail;
			NN<UI::GUIListView> lvMail;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<Net::MSGraphEventMessageRequest> msgList;
			Optional<Net::SSLEngine> ssl;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;

			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			void DisplayRecipient(NN<Text::StringBuilderUTF8> sb, NN<Net::MSGraphRecipient> rcpt);
			void UpdateMessages();
		public:
			AVIRMSGraphEmailForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMSGraphEmailForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
