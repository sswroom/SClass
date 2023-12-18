#ifndef _SM_SSWR_AVIREAD_AVIRLDAPEXPLORERFORM
#define _SM_SSWR_AVIREAD_AVIRLDAPEXPLORERFORM
#include "Net/LDAPClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLDAPExplorerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::LDAPClient *cli;
			Data::ArrayList<Net::LDAPClient::SearchResObject*> *dispResults;
			UOSInt rootLev;

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
			NotNullPtr<UI::GUIButton> btnConnect;
			UI::GUIListBox *lbPath;
			NotNullPtr<UI::GUIHSplitter> hspPath;
			UI::GUIListBox *lbObjects;
			NotNullPtr<UI::GUIHSplitter> hspObjects;
			UI::GUIListView *lvValues;
		
			static void __stdcall OnConnectClicked(void *userObj);
			static void __stdcall OnPathSelChg(void *userObj);
			static void __stdcall OnObjectsSelChg(void *userObj);
			static void __stdcall OnObjectsDblClk(void *userObj);

		public:
			AVIRLDAPExplorerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
