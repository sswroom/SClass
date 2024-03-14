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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::LDAPClient *cli;
			Data::ArrayList<Net::LDAPClient::SearchResObject*> *dispResults;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblAuthType;
			NotNullPtr<UI::GUIComboBox> cboAuthType;
			NotNullPtr<UI::GUILabel> lblUserDN;
			NotNullPtr<UI::GUITextBox> txtUserDN;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUIButton> btnConnect;
			NotNullPtr<UI::GUIGroupBox> grpSearch;
			NotNullPtr<UI::GUIPanel> pnlSearchRequest;
			NotNullPtr<UI::GUILabel> lblSearchBase;
			NotNullPtr<UI::GUITextBox> txtSearchBase;
			NotNullPtr<UI::GUILabel> lblSearchScope;
			NotNullPtr<UI::GUIComboBox> cboSearchScope;
			NotNullPtr<UI::GUILabel> lblSearchDerefAliases;
			NotNullPtr<UI::GUIComboBox> cboSearchDerefAliases;
			NotNullPtr<UI::GUILabel> lblSearchFilter;
			NotNullPtr<UI::GUITextBox> txtSearchFilter;
			NotNullPtr<UI::GUIButton> btnSearch;
			NotNullPtr<UI::GUIComboBox> cboSearchResult;
			NotNullPtr<UI::GUIListView> lvSearch;
		
			static void __stdcall OnConnectClicked(AnyType userObj);
			static void __stdcall OnSearchClicked(AnyType userObj);
			static void __stdcall OnSearchResultSelChg(AnyType userObj);

		public:
			AVIRLDAPClientForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
