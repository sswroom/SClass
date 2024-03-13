#ifndef _SM_SSWR_AVIREAD_AVIRSNMPMANAGERFORM
#define _SM_SSWR_AVIREAD_AVIRSNMPMANAGERFORM
#include "Net/SNMPManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/SMonitor/SMonitorRedir.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
		class AVIRSNMPManagerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			SSWR::SMonitor::SMonitorRedir *redir;
			Net::SNMPManager *mgr;
			Int64 lastUpdateTime;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUILabel> lblAgentAddr;
			NotNullPtr<UI::GUITextBox> txtAgentAddr;
			NotNullPtr<UI::GUICheckBox> chkAgentScan;
			NotNullPtr<UI::GUILabel> lblCommunity;
			NotNullPtr<UI::GUITextBox> txtCommunity;
			NotNullPtr<UI::GUIButton> btnAgentAdd;
			NotNullPtr<UI::GUICheckBox> chkSendToSvr;
			NotNullPtr<UI::GUIListBox> lbAgent;
			NotNullPtr<UI::GUIHSplitter> hspAgent;
			NotNullPtr<UI::GUIPanel> pnlAgent;
			NotNullPtr<UI::GUILabel> lblAgentDAddr;
			NotNullPtr<UI::GUITextBox> txtAgentDAddr;
			NotNullPtr<UI::GUIButton> btnAgentWalk;
			NotNullPtr<UI::GUILabel> lblAgentDescr;
			NotNullPtr<UI::GUITextBox> txtAgentDescr;
			NotNullPtr<UI::GUILabel> lblAgentOID;
			NotNullPtr<UI::GUITextBox> txtAgentOID;
			NotNullPtr<UI::GUILabel> lblAgentOIDName;
			NotNullPtr<UI::GUITextBox> txtAgentOIDName;
			NotNullPtr<UI::GUILabel> lblAgentName;
			NotNullPtr<UI::GUITextBox> txtAgentName;
			NotNullPtr<UI::GUILabel> lblAgentContact;
			NotNullPtr<UI::GUITextBox> txtAgentContact;
			NotNullPtr<UI::GUILabel> lblAgentLocation;
			NotNullPtr<UI::GUITextBox> txtAgentLocation;
			NotNullPtr<UI::GUILabel> lblAgentPhyAddr;
			NotNullPtr<UI::GUITextBox> txtAgentPhyAddr;
			NotNullPtr<UI::GUILabel> lblAgentVendor;
			NotNullPtr<UI::GUITextBox> txtAgentVendor;
			NotNullPtr<UI::GUILabel> lblAgentModel;
			NotNullPtr<UI::GUITextBox> txtAgentModel;
			NotNullPtr<UI::GUIListView> lvAgentReading;

			static void __stdcall OnAgentAddClicked(void *userObj);
			static void __stdcall OnAgentSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAgentWalkClicked(void *userObj);
			void SendAgentValues(NotNullPtr<Data::ArrayList<Net::SNMPManager::AgentInfo *>> agentList);

		public:
			AVIRSNMPManagerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPManagerForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
