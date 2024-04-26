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
			NN<SSWR::AVIRead::AVIRCore> core;
			SSWR::SMonitor::SMonitorRedir *redir;
			Net::SNMPManager *mgr;
			Int64 lastUpdateTime;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUILabel> lblAgentAddr;
			NN<UI::GUITextBox> txtAgentAddr;
			NN<UI::GUICheckBox> chkAgentScan;
			NN<UI::GUILabel> lblCommunity;
			NN<UI::GUITextBox> txtCommunity;
			NN<UI::GUIButton> btnAgentAdd;
			NN<UI::GUICheckBox> chkSendToSvr;
			NN<UI::GUIListBox> lbAgent;
			NN<UI::GUIHSplitter> hspAgent;
			NN<UI::GUIPanel> pnlAgent;
			NN<UI::GUILabel> lblAgentDAddr;
			NN<UI::GUITextBox> txtAgentDAddr;
			NN<UI::GUIButton> btnAgentWalk;
			NN<UI::GUILabel> lblAgentDescr;
			NN<UI::GUITextBox> txtAgentDescr;
			NN<UI::GUILabel> lblAgentOID;
			NN<UI::GUITextBox> txtAgentOID;
			NN<UI::GUILabel> lblAgentOIDName;
			NN<UI::GUITextBox> txtAgentOIDName;
			NN<UI::GUILabel> lblAgentName;
			NN<UI::GUITextBox> txtAgentName;
			NN<UI::GUILabel> lblAgentContact;
			NN<UI::GUITextBox> txtAgentContact;
			NN<UI::GUILabel> lblAgentLocation;
			NN<UI::GUITextBox> txtAgentLocation;
			NN<UI::GUILabel> lblAgentPhyAddr;
			NN<UI::GUITextBox> txtAgentPhyAddr;
			NN<UI::GUILabel> lblAgentVendor;
			NN<UI::GUITextBox> txtAgentVendor;
			NN<UI::GUILabel> lblAgentModel;
			NN<UI::GUITextBox> txtAgentModel;
			NN<UI::GUIListView> lvAgentReading;

			static void __stdcall OnAgentAddClicked(AnyType userObj);
			static void __stdcall OnAgentSelChg(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnAgentWalkClicked(AnyType userObj);
			void SendAgentValues(NN<Data::ArrayListNN<Net::SNMPManager::AgentInfo>> agentList);

		public:
			AVIRSNMPManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSNMPManagerForm();

			virtual void OnMonitorChanged();

			Bool IsError();
		};
	}
}
#endif
