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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::LDAPClient> cli;
			Data::ArrayListNN<Net::LDAPClient::SearchResObject> dispResults;
			UIntOS rootLev;

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
			NN<UI::GUIListBox> lbPath;
			NN<UI::GUIHSplitter> hspPath;
			NN<UI::GUIListBox> lbObjects;
			NN<UI::GUIHSplitter> hspObjects;
			NN<UI::GUIListView> lvValues;
		
			static void __stdcall OnConnectClicked(AnyType userObj);
			static void __stdcall OnPathSelChg(AnyType userObj);
			static void __stdcall OnObjectsSelChg(AnyType userObj);
			static void __stdcall OnObjectsDblClk(AnyType userObj);

		public:
			AVIRLDAPExplorerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLDAPExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
