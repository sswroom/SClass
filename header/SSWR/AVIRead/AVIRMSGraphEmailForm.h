#ifndef _SM_SSWR_AVIREAD_AVIRMSGRAPHEMAILFORM
#define _SM_SSWR_AVIREAD_AVIRMSGRAPHEMAILFORM
#include "Net/MSGraphClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpRead;
			NN<UI::GUIPanel> pnlRead;
			NN<UI::GUIButton> btnRead;
			NN<UI::GUIListView> lvRead;

			NN<UI::GUITabPage> tpSend;
			NN<UI::GUIPanel> pnlSendPre;
			NN<UI::GUIPanel> pnlSendPost;
			NN<UI::GUILabel> lblSendTo;
			NN<UI::GUITextBox> txtSendTo;
			NN<UI::GUILabel> lblSendCC;
			NN<UI::GUITextBox> txtSendCC;
			NN<UI::GUILabel> lblSendBCC;
			NN<UI::GUITextBox> txtSendBCC;
			NN<UI::GUILabel> lblSendSubject;
			NN<UI::GUITextBox> txtSendSubject;
			NN<UI::GUICheckBox> chkSendHTML;
			NN<UI::GUITextBox> txtSendContent;
			NN<UI::GUILabel> lblSendAtt;
			NN<UI::GUITextBox> txtSendAtt;
			NN<UI::GUIButton> btnSendAttAdd;
			NN<UI::GUIButton> btnSendAttClear;
			NN<UI::GUIButton> btnSend;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIListBox> lbLog;

			NN<SSWR::AVIRead::AVIRCore> core;
			Data::ArrayListNN<Net::MSGraphEventMessageRequest> msgList;
			Optional<Net::SSLEngine> ssl;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Data::ArrayListStringNN atts;
			Optional<Net::MSGraphAccessToken> token;

			static void __stdcall OnReadClicked(AnyType userObj);
			static void __stdcall OnSendClicked(AnyType userObj);
			static void __stdcall OnSendAttAddClicked(AnyType userObj);
			static void __stdcall OnSendAttClearClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			void DisplayRecipient(NN<Text::StringBuilderUTF8> sb, NN<Net::MSGraphRecipient> rcpt);
			void UpdateMessages();
			Optional<Net::MSGraphAccessToken> GetToken(NN<Net::MSGraphClient> cli);
		public:
			AVIRMSGraphEmailForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMSGraphEmailForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
