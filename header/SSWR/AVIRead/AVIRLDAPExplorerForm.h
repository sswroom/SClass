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
			NotNullPtr<UI::GUIListBox> lbPath;
			NotNullPtr<UI::GUIHSplitter> hspPath;
			NotNullPtr<UI::GUIListBox> lbObjects;
			NotNullPtr<UI::GUIHSplitter> hspObjects;
			NotNullPtr<UI::GUIListView> lvValues;
		
			static void __stdcall OnConnectClicked(AnyType userObj);
			static void __stdcall OnPathSelChg(AnyType userObj);
			static void __stdcall OnObjectsSelChg(AnyType userObj);
			static void __stdcall OnObjectsDblClk(AnyType userObj);

		public:
			AVIRLDAPExplorerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
