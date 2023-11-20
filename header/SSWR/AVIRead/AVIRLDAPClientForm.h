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
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblAuthType;
			UI::GUIComboBox *cboAuthType;
			UI::GUILabel *lblUserDN;
			UI::GUITextBox *txtUserDN;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUIButton *btnConnect;
			NotNullPtr<UI::GUIGroupBox> grpSearch;
			NotNullPtr<UI::GUIPanel> pnlSearchRequest;
			UI::GUILabel *lblSearchBase;
			UI::GUITextBox *txtSearchBase;
			UI::GUILabel *lblSearchScope;
			UI::GUIComboBox *cboSearchScope;
			UI::GUILabel *lblSearchDerefAliases;
			UI::GUIComboBox *cboSearchDerefAliases;
			UI::GUILabel *lblSearchFilter;
			UI::GUITextBox *txtSearchFilter;
			UI::GUIButton *btnSearch;
			UI::GUIComboBox *cboSearchResult;
			UI::GUIListView *lvSearch;
		
			static void __stdcall OnConnectClicked(void *userObj);
			static void __stdcall OnSearchClicked(void *userObj);
			static void __stdcall OnSearchResultSelChg(void *userObj);

		public:
			AVIRLDAPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPClientForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
