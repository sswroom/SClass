#ifndef _SM_SSWR_AVIREAD_AVIRLDAPCLIENTFORM
#define _SM_SSWR_AVIREAD_AVIRLDAPCLIENTFORM
#include "Net/LDAPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLDAPClientForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::LDAPClient *cli;
			Data::ArrayListNN<Net::LDAPClient::SearchResObject> dispResults;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblAuthType;
			NN<UI::GUIComboBox> cboAuthType;
			NN<UI::GUILabel> lblUserDN;
			NN<UI::GUITextBox> txtUserDN;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUIButton> btnConnect;
			NN<UI::GUIGroupBox> grpSearch;
			NN<UI::GUIPanel> pnlSearchRequest;
			NN<UI::GUILabel> lblSearchBase;
			NN<UI::GUITextBox> txtSearchBase;
			NN<UI::GUILabel> lblSearchScope;
			NN<UI::GUIComboBox> cboSearchScope;
			NN<UI::GUILabel> lblSearchDerefAliases;
			NN<UI::GUIComboBox> cboSearchDerefAliases;
			NN<UI::GUILabel> lblSearchFilter;
			NN<UI::GUITextBox> txtSearchFilter;
			NN<UI::GUIButton> btnSearch;
			NN<UI::GUIComboBox> cboSearchResult;
			NN<UI::GUIListView> lvSearch;
		
			static void __stdcall OnConnectClicked(AnyType userObj);
			static void __stdcall OnSearchClicked(AnyType userObj);
			static void __stdcall OnSearchResultSelChg(AnyType userObj);

		public:
			AVIRLDAPClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
