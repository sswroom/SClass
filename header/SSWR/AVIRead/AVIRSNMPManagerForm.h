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

			UI::GUIPanel *pnlControl;
			UI::GUILabel *lblAgentAddr;
			UI::GUITextBox *txtAgentAddr;
			UI::GUICheckBox *chkAgentScan;
			UI::GUILabel *lblCommunity;
			UI::GUITextBox *txtCommunity;
			UI::GUIButton *btnAgentAdd;
			UI::GUICheckBox *chkSendToSvr;
			UI::GUIListBox *lbAgent;
			UI::GUIHSplitter *hspAgent;
			UI::GUIPanel *pnlAgent;
			UI::GUILabel *lblAgentDAddr;
			UI::GUITextBox *txtAgentDAddr;
			UI::GUIButton *btnAgentWalk;
			UI::GUILabel *lblAgentDescr;
			UI::GUITextBox *txtAgentDescr;
			UI::GUILabel *lblAgentOID;
			UI::GUITextBox *txtAgentOID;
			UI::GUILabel *lblAgentOIDName;
			UI::GUITextBox *txtAgentOIDName;
			UI::GUILabel *lblAgentName;
			UI::GUITextBox *txtAgentName;
			UI::GUILabel *lblAgentContact;
			UI::GUITextBox *txtAgentContact;
			UI::GUILabel *lblAgentLocation;
			UI::GUITextBox *txtAgentLocation;
			UI::GUILabel *lblAgentPhyAddr;
			UI::GUITextBox *txtAgentPhyAddr;
			UI::GUILabel *lblAgentVendor;
			UI::GUITextBox *txtAgentVendor;
			UI::GUILabel *lblAgentModel;
			UI::GUITextBox *txtAgentModel;
			UI::GUIListView *lvAgentReading;

			static void __stdcall OnAgentAddClicked(void *userObj);
			static void __stdcall OnAgentSelChg(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnAgentWalkClicked(void *userObj);
			void SendAgentValues(Data::ArrayList<Net::SNMPManager::AgentInfo *> *agentList);

		public:
			AVIRSNMPManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPManagerForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
